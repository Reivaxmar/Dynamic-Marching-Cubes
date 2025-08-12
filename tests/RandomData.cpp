#include <asio.hpp>
#include <iostream>
#include <thread>

#include <random>
#include <vector>
#include <glm/glm.hpp>
#include <cstring>
#include <cstdint>
#include <arpa/inet.h>
#include <endian.h>

int main() {
    try {
        asio::io_context io;
        asio::ip::tcp::socket socket(io);

        asio::ip::tcp::endpoint endpoint(asio::ip::make_address("0.0.0.0"), 5000);

        socket.connect(endpoint);

        int frameID = 0, pointCount = 1000;
        auto start = std::chrono::system_clock::now();

        while(true) {
            std::vector<uint8_t> buffer;

            // Add magic header
            buffer.insert(buffer.end(), {'P','C','L','D'});

            // Add version
            buffer.push_back(1);

            // Add frame ID
            uint32_t netFrameID = htonl(frameID);
            buffer.insert(buffer.end(),
                        reinterpret_cast<uint8_t*>(&netFrameID),
                        reinterpret_cast<uint8_t*>(&netFrameID) + sizeof(netFrameID));

            // Add timestamp
            std::chrono::duration<double> elapsed_seconds = std::chrono::system_clock::now() - start;
            double timestamp = elapsed_seconds.count();
            uint64_t timestampBits;
            std::memcpy(&timestampBits, &timestamp, sizeof(timestampBits));
            timestampBits = htobe64(timestampBits);
            buffer.insert(buffer.end(),
                        reinterpret_cast<uint8_t*>(&timestampBits),
                        reinterpret_cast<uint8_t*>(&timestampBits) + sizeof(timestampBits));

            // Add point count
            uint32_t netPointCount = htonl(pointCount);
            buffer.insert(buffer.end(),
                        reinterpret_cast<uint8_t*>(&netPointCount),
                        reinterpret_cast<uint8_t*>(&netPointCount) + sizeof(netPointCount));

            // Add random points (0-127)
            std::mt19937 rng(std::random_device{}());
            std::uniform_real_distribution<float> dist(0.f, 127.f);

            for (uint32_t i = 0; i < pointCount; i++) {
                glm::vec3 p(dist(rng), dist(rng), dist(rng));
                buffer.insert(buffer.end(),
                            reinterpret_cast<uint8_t*>(&p),
                            reinterpret_cast<uint8_t*>(&p) + sizeof(p));
            }

            // Send the data
            asio::write(socket, asio::buffer(buffer));

            frameID++;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        // std::string message = "Hello from CMake test!";
        // asio::write(socket, asio::buffer(message));

        // std::cout << "Data sent successfully\n";
        return 0; // success
    } catch (std::exception &e) {
        std::cerr << "Test failed: " << e.what() << "\n";
        return 1; // failure
    }
}
