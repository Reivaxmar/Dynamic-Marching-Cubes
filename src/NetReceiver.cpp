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

bool NetReceiver::GetPointCloud(glm::mat4& camMat, std::vector<Point>& points) {
    // Lock the queue
    std::unique_lock<std::mutex> lock(queueMutex);
    // Check if there's anything new
    if (!PCqueue.empty()) {
        camMat = PCqueue.front().first;
        points = std::move(PCqueue.front().second);
        // points = std::move(std::get<1>(PCqueue.front()));
        // colors = std::move(std::get<2>(PCqueue.front()));
        PCqueue.pop();
        return true;
    }
    return false;
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

    // Read frame ID
    uint32_t frameID;
    if (!read_exact(socket, &frameID, 4)) return false;

    // Read timestamp
    double timestamp;
    if (!read_exact(socket, &timestamp, 8)) return false;

    // Read camera matrix
    glm::mat4 camMat;
    if (!read_exact(socket, &camMat, sizeof(camMat))) return false;

    // Read point cloud count
    uint32_t pointCount;
    if (!read_exact(socket, &pointCount, 4)) return false;

    // Read point positions
    std::vector<glm::vec3> points(pointCount);
    if (!read_exact(socket, points.data(), pointCount * sizeof(glm::vec3))) return false;

    // Read point colors
    std::vector<glm::vec3> colors(pointCount);
    if (!read_exact(socket, colors.data(), pointCount * sizeof(glm::vec3))) return false;

    if(isCalibrating) {
        // Update bounding box
        for(const auto& p : points) {
            if(glm::length2(p) > 100) continue; // More than 10m away (mistake)
            if (!std::isfinite(p.x) || !std::isfinite(p.y) || !std::isfinite(p.z)) continue; // Error
            
            minBB = glm::vec3(std::min(minBB.x, p.x), std::min(minBB.y, p.y), std::min(minBB.z, p.z));
            maxBB = glm::vec3(std::max(maxBB.x, p.x), std::max(maxBB.y, p.y), std::max(maxBB.z, p.z));
        }
    } else {
        // Compute uniform scale and offset
        glm::vec3 bbSize = maxBB - minBB;
        float maxDim = std::max({bbSize.x, bbSize.y, bbSize.z});
        float scale = (maxDim > 0.0f) ? (128.0f / maxDim) : 1.0f;
        glm::vec3 offset = -minBB;

        // Build transformation matrix
        glm::mat4 transform(1.0f);

        // Note: order matters! If you want (p + offset) * scale, you need to apply
        // translate first, then scale. GLM composes right-to-left, so the code above
        // actually does scale first, then translate.
        transform = glm::scale(glm::mat4(1.0f), glm::vec3(scale)) *
                    glm::translate(glm::mat4(1.0f), offset);

        // Transform all points
        // std::vector<glm::vec4> points4(pointCount), colors4(pointCount);
        std::vector<Point> point_cloud(pointCount);
        for (int i = 0; i < pointCount; i++) {
            point_cloud[i] = Point {
                transform * glm::vec4(points[i], 1.0f),
                glm::vec4(colors[i], 1.0f)
            };
        }

        // Transform camera position
        camMat = transform * camMat;

        // Push to queue
        {
            std::lock_guard<std::mutex> lock(queueMutex);
            // PCqueue.push(std::move(std::make_tuple(camMat, points4, colors4)));
            PCqueue.push(std::move(std::make_pair(camMat, point_cloud)));
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
