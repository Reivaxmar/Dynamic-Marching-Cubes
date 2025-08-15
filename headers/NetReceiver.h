#ifndef NET_RECEIVER_CLASS_H
#define NET_RECEIVER_CLASS_H

#include <queue>
#include <mutex>
#include <thread>
#include <iostream>

#include <asio.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

using asio::ip::tcp;

class NetReceiver {
public:
    NetReceiver(unsigned short port);
    ~NetReceiver();
    void GetPointCloud(std::vector<glm::vec4>& in);

private:
    bool done = false;
    std::queue<std::vector<glm::vec4>> PCqueue;
    std::mutex queueMutex;
    std::thread dataThread;

    asio::io_context io;
    tcp::acceptor acceptor;
    tcp::socket socket;
    asio::executor_work_guard<asio::io_context::executor_type> work_guard;
    unsigned short port;

    bool readPointCloud();
    inline bool read_exact(tcp::socket& socket, void* buffer, std::size_t length);
    void startAccept();
    void startReadLoop();


};

#endif // NET_RECEIVER_CLASS_H