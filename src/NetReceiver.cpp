#include "NetReceiver.h"

NetReceiver::NetReceiver(unsigned short port)
    : work_guard(asio::make_work_guard(io))
    , acceptor(io, tcp::endpoint(tcp::v4(), port))
    , socket(io)
    , port(port)
{
    startAccept(); // post accept before io.run() starts
    dataThread = std::thread([this]() { io.run(); });
}

NetReceiver::~NetReceiver() {
    std::cout << "Exiting\n";
    done = true;
    
    work_guard.reset();
    
    // Close connection
    asio::post(io, [this]() {
        asio::error_code ec;
        acceptor.close(ec);
        socket.close(ec);
    });

    // Close thread
    if (dataThread.joinable())
        dataThread.join();
}

void NetReceiver::GetPointCloud(std::vector<glm::vec4>& in) {
    // Lock the queue
    std::unique_lock<std::mutex> lock(queueMutex);
    // Check if there's anything new
    if (!PCqueue.empty()) {
        in.swap(PCqueue.front());
        PCqueue.pop();
    }
}

bool NetReceiver::readPointCloud() {

    // Read header
    char magic[4];
    if (!read_exact(socket, magic, 4)) return false;
    if (std::memcmp(magic, "PCLD", 4) != 0) {
        std::cerr << "[Network] Invalid magic, closing. It was: " << magic << "\n";
        return false;
    }

    // Read version
    uint8_t version;
    if (!read_exact(socket, &version, 1)) return false;
    if (version != 1) {
        std::cerr << "[Network] Unsupported version " << int(version) << "\n";
        return false;
    }

    // Read frame ID
    uint32_t netFrameID;
    if (!read_exact(socket, &netFrameID, 4)) return false;
    uint32_t frameID = ntohl(netFrameID);

    // Read timestamp
    uint64_t netTimestampBits;
    if (!read_exact(socket, &netTimestampBits, 8)) return false;
    netTimestampBits = be64toh(netTimestampBits);
    double timestamp;
    std::memcpy(&timestamp, &netTimestampBits, sizeof(timestamp));

    // Read point cloud count
    uint32_t netPointCount;
    if (!read_exact(socket, &netPointCount, 4)) return false;
    uint32_t pointCount = ntohl(netPointCount);

    // Read points
    std::vector<glm::vec4> points(pointCount);
    if (!read_exact(socket, points.data(), pointCount * sizeof(glm::vec4))) return false;

    for(auto& p : points) {
        // Transform the points
        p = (p + glm::vec4(1.f)) * 32.f;
    }

    // Push to queue
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        PCqueue.push(std::move(points));
    }

    // Print information
    std::cout << "[Network] Frame " << frameID
              << " @ t=" << timestamp
              << "s, points=" << pointCount << "\n";
    return true;
}

inline bool NetReceiver::read_exact(tcp::socket& socket, void* buffer, std::size_t length) {
    asio::error_code ec;
    std::size_t total = 0;
    char* buf = static_cast<char*>(buffer);
    while (total < length && !ec) {
        std::size_t n = socket.read_some(asio::buffer(buf + total, length - total), ec);
        if (ec) return false;
        total += n;
    }
    return total == length;
}

void NetReceiver::startAccept() {
    std::cout << "[Network] Starting async_accept...\n";
    // Start an async connection accept
    acceptor.async_accept(socket, 
        [this](asio::error_code ec) {
            if (done) return;
            if (!ec) {
                std::cout << "[Network] Client connected: "
                          << socket.remote_endpoint() << "\n";
                startReadLoop();
            } else {
                std::cerr << "[Network] async_accept fired: " << ec.message() << "\n";
            }
        }
    );
}

void NetReceiver::startReadLoop() {
    // Once connected, read the data
    std::thread([this]() {
        while (!done) {
            if (!readPointCloud()) break;
        }
    }).detach();
}
