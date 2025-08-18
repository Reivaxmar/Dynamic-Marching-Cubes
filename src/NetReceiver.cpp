#include "NetReceiver.h"

NetReceiver::NetReceiver(unsigned short port)
    : work_guard(asio::make_work_guard(io))
    , acceptor(io, tcp::endpoint(tcp::v4(), port))
    , socket(io)
    , port(port)
{
    // Init bounding box
    minBB = glm::vec3(std::numeric_limits<float>::max());
    maxBB = glm::vec3(std::numeric_limits<float>::lowest());

    startAccept(); // post accept before io.run() starts
    dataThread = std::thread([this]() { io.run(); });
}

NetReceiver::~NetReceiver() {
    std::cout << "[Network] Exiting\n";
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

bool NetReceiver::IsCalibrating() {
    return isCalibrating;
}

bool NetReceiver::readPointCloud() {

    // Read header
    char magic[4];
    if (!read_exact(socket, magic, 4)) return false;
    if (std::memcmp(magic, "PCLD", 4) != 0) {
        std::cerr << "[Network] Invalid magic, closing. It was: " << magic << "\n";
        return false;
    }

    // Read if calibrating
    uint8_t calibrating;
    if (!read_exact(socket, &calibrating, 1)) return false;
    isCalibrating = calibrating;
    // if (calibrating != 1) {
    //     std::cerr << "[Network] Unsupported version " << int(version) << "\n";
    //     return false;
    // }

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
    uint32_t pointCount;
    if (!read_exact(socket, &pointCount, 4)) return false;
    // uint32_t pointCount = ntohl(netPointCount);

    // Read points
    std::vector<glm::vec3> points(pointCount);
    if (!read_exact(socket, points.data(), pointCount * sizeof(glm::vec3))) return false;

    if(isCalibrating) {
        // Update bounding box
        float mx = -999;
        for(const auto& p : points) {
            if(glm::length2(p) > 100) continue; // More than 10m away (mistake)
            if (!std::isfinite(p.x) || !std::isfinite(p.y) || !std::isfinite(p.z)) continue; // Error
            
            minBB = glm::vec3(std::min(minBB.x, p.x), std::min(minBB.y, p.y), std::min(minBB.z, p.z));
            maxBB = glm::vec3(std::max(maxBB.x, p.x), std::max(maxBB.y, p.y), std::max(maxBB.z, p.z));
            if(p.x > mx) {
                std::cout << "New p.x: " << p.x << "\n";
                mx = std::max(mx, p.x);
            }
        }
        std::cout << "mx: " << mx << std::endl;
    } else {
        // Convert to glm::vec4
        std::vector<glm::vec4> points4(pointCount);
    
        for(int i = 0; i < pointCount; i++) {
            // Transform the points
            points4[i] = glm::vec4((points[i] - minBB) / (maxBB - minBB) * 128.0f, 1.f);
            // std::cout << "New point: " << points4[i].x << ", " << points4[i].y << ", " << points4[i].z << "\n";
            // points4[i] = glm::vec4((points[i] + glm::vec3(1.f)) * 32.f, 1.f);
        }
        std::cout << "ASDASDSA: "<< glm::length2(maxBB) << "\n";
        std::cout << "minBB: " << minBB.x << ", " << minBB.y << ", " << minBB.z << "\n";
        std::cout << "maxBB: " << maxBB.x << ", " << maxBB.y << ", " << maxBB.z << "\n\n";
    
        // Push to queue
        {
            std::lock_guard<std::mutex> lock(queueMutex);
            PCqueue.push(std::move(points4));
        }
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
