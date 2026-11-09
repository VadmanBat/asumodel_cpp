#pragma once
// Created by Vadim 18.01.2026.

#include <string>
#include <thread>
#include <atomic>
#include <cstddef>      // для size_t

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>   // для inet_pton
#else
#include <sys/socket.h>
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <unistd.h>
    #include <fcntl.h>
#endif

class UDPHandler {
public:
    bool init(int port = 5000, const std::string& host = "127.0.0.1");
    void send_binary(const void* data, size_t size);
    void stop();

private:
    void receive_thread();

    int sock = -1;
    sockaddr_in addr{};                 // теперь тип известен
    std::thread recv_thread;
    std::atomic<bool> running{true};
};