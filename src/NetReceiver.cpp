#include "NetReceiver.h"

NetReceiver::NetReceiver(unsigned short port)
    : acceptor(io, tcp::endpoint(tcp::v4(), port))
    , socket(io)
    , dataThread([this, port]() { getDataThread(port); })
{
    
}

NetReceiver::~NetReceiver() {
    done = true;
    dataThread.join();
}

void NetReceiver::GetPointCloud(std::vector<glm::vec4>& in) {
    // Lock the queue
    std::unique_lock<std::mutex> lock(queueMutex);
    // Check if there's anything new
    if(!PCqueue.empty()) {
        in = std::move(PCqueue.front());
        PCqueue.pop();
    }
}

void NetReceiver::getDataThread(unsigned short port) {

    acceptor.non_blocking(true);

    // Start listening
    std::cout << "[Network] Listening on port " << port << "...\n";
    // Loop until accepted
    while(!done) {
        asio::error_code ec;
        acceptor.accept(socket, ec);
        if (!ec) {
            std::cout << "[Network] Client connected: " << socket.remote_endpoint() << "\n";
            break;
        }
        if (ec != asio::error::would_block) {
            std::cerr << "[Network] Accept error: " << ec.message() << "\n";
            return;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }


    while(!done) {

        // Read header
        char magic[4];
        if (!read_exact(socket, magic, 4)) break;
        if (std::memcmp(magic, "PCLD", 4) != 0) {
            std::cerr << "[Network] Invalid magic, closing.\n";
            break;
        }

        // Read version
        uint8_t version;
        if (!read_exact(socket, &version, 1)) break;
        if (version != 1) {
            std::cerr << "[Network] Unsupported version " << int(version) << "\n";
            break;
        }

        // Read frame ID
        uint32_t netFrameID;
        if (!read_exact(socket, &netFrameID, 4)) break;
        uint32_t frameID = ntohl(netFrameID);

        // Read timestamp
        uint64_t netTimestampBits;
        if (!read_exact(socket, &netTimestampBits, 8)) break;
        netTimestampBits = be64toh(netTimestampBits);
        double timestamp;
        std::memcpy(&timestamp, &netTimestampBits, sizeof(timestamp));

        // Read point cloud count
        uint32_t netPointCount;
        if (!read_exact(socket, &netPointCount, 4)) break;
        uint32_t pointCount = ntohl(netPointCount);

        // Read points
        std::vector<glm::vec3> points3(pointCount);
        if (!read_exact(socket, points3.data(), pointCount * sizeof(glm::vec3))) break;

        // Convert to vec4
        std::vector<glm::vec4> points4;
        points4.reserve(pointCount);
        for (const auto& p : points3) {
            points4.emplace_back(p, 1.0f);
        }

        // Push to queue
        {
            std::lock_guard<std::mutex> lock(queueMutex);
            PCqueue.push(std::move(points4));
        }

        // Print information
        std::cout << "[Network] Frame " << frameID
                  << " @ t=" << timestamp
                  << "s, points=" << pointCount << "\n";
    }
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
