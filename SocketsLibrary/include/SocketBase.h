#pragma once

#include <string>

#include "NetSocket.h"

namespace Net {
    class SocketBase
    {
    public:
        struct SocketHandle {
            Net::socket_t socket;
            sockaddr_in addr;

            void CloseSocket(void) noexcept;
            socket_t CreateSocket(void);

            SocketHandle(const SocketHandle&) = delete;
            SocketHandle& operator=(const SocketHandle&) = delete;

            SocketHandle(SocketHandle&& other) noexcept
                : socket(other.socket), addr(other.addr)
            {
                other.socket = Net::INVALID_SOCKET_VALUE; // prevent double close
            }

            SocketHandle& operator=(SocketHandle&& other) noexcept {
                if (this != &other) {
                    CloseSocket(); // clean up any existing socket
                    socket = other.socket;
                    addr = other.addr;
                    other.socket = Net::INVALID_SOCKET_VALUE;
                }
                return *this;
            }

            ~SocketHandle();
            SocketHandle();
        };


        virtual ~SocketBase() = 0;
    };
    inline SocketBase::~SocketBase() {};
}

