#include "udp_handler.h"
#include <iostream>
#include <cstring>      // для memset

#ifdef _WIN32
#pragma comment(lib, "ws2_32.lib")
#else
// на Linux ничего дополнительно не нужно
#endif

bool UDPHandler::init(int port, const std::string& host) {
#ifdef _WIN32
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        std::cerr << "WSAStartup failed\n";
        return false;
    }
#endif

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        std::cerr << "socket() failed\n";
        return false;
    }

    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, host.c_str(), &addr.sin_addr);

    recv_thread = std::thread(&UDPHandler::receive_thread, this);
    return true;
}

void UDPHandler::send_binary(const void* data, size_t size) {
    sendto(sock, static_cast<const char*>(data), static_cast<int>(size), 0,
           reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr));
}

void UDPHandler::receive_thread() {
    char buf[65536];
    while (running) {
#ifdef _WIN32
        int addr_len = sizeof(addr);
#else
        socklen_t addr_len = sizeof(addr);
#endif
        int n = recvfrom(sock, buf, sizeof(buf), 0,
                         reinterpret_cast<struct sockaddr*>(&addr),
                         &addr_len);
        if (n > 0) {
            std::cout << "[UDP] Received " << n << " bytes\n";
            // здесь можно добавить обработку входящих команд
        }
    }
}

void UDPHandler::stop() {
    running = false;
    if (recv_thread.joinable()) {
        recv_thread.join();
    }

#ifdef _WIN32
    closesocket(sock);
    WSACleanup();
#else
    close(sock);
#endif
    sock = -1;
}