#pragma once

#ifdef _WIN32
#define NOMINMAX
#include <WinSock2.h>
#include <ws2tcpip.h> // For inet_pton (Windows specific)
#include <windows.h>
#pragma comment(lib, "Ws2_32.lib")
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#endif

#include <iostream>
#include <string>

namespace Net
{
    /* --- Portable types and constants --- */
#ifdef _WIN32
    using socket_t = SOCKET;
    using socklen = int;
#else
    using socket_t = int;
    using socklen = socklen_t;
#endif

    /* --- Shutdown constants --- */
#ifdef _WIN32
    constexpr int SHUT_READ = SD_RECEIVE;
    constexpr int SHUT_WRITE = SD_SEND;
    constexpr int SHUT_BOTH = SD_BOTH;
#else
    constexpr int SHUT_READ = SHUT_RD;
    constexpr int SHUT_WRITE = SHUT_WR;
    constexpr int SHUT_BOTH = SHUT_RDWR;
#endif

    /* --- Socket error codes --- */
#ifdef _WIN32
    constexpr socket_t INVALID_SOCKET_VALUE = INVALID_SOCKET;
    constexpr int SOCKET_ERROR_VALUE = SOCKET_ERROR;
    constexpr int SOCKET_ALREADY_CLOSED = WSAENOTSOCK;
    constexpr int SOCKET_ERROR_WOULDBLOCK = WSAEWOULDBLOCK;
    constexpr int SOCKET_ERROR_AGAIN = WSAEWOULDBLOCK;
    constexpr int SOCKET_ERROR_INTR = WSAEINTR;
#else
    constexpr socket_t INVALID_SOCKET_VALUE = -1;
    constexpr int SOCKET_ERROR_VALUE = -1;
    constexpr int SOCKET_ALREADY_CLOSED = EBADF;
    constexpr int SOCKET_ERROR_WOULDBLOCK = EWOULDBLOCK;
    constexpr int SOCKET_ERROR_AGAIN = EAGAIN;
    constexpr int SOCKET_ERROR_INTR = EINTR;
#endif

    inline int CloseSocket(socket_t socket) {
#ifdef _WIN32
        return closesocket(socket);
#else
        return close(socket);
#endif
    }

    /* --- Startup and Cleanup --- */
    inline int startup()
    {
#ifdef _WIN32
        WSADATA WsaData;
        return WSAStartup(MAKEWORD(2, 2), &WsaData);
#else
        return 0; // No-op on POSIX
#endif
    }

    inline int cleanup()
    {
#ifdef _WIN32
        return WSACleanup();
#else
        return 0; // No-op on POSIX
#endif
    }

    inline void sleep_ms(size_t ms) {
#ifdef _WIN32
        Sleep(ms);
#else
        usleep(ms);
#endif
    }

    inline void Info(const std::string& msg) {
        std::cerr << "[INFO] " << msg << std::endl;
    };
    inline void Error(const std::string& msg) {
        std::cerr << "[ERROR] " << msg << std::endl;
    };

}