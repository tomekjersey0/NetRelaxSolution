#pragma once

#include <string>

#include "Program.h"
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

            ~SocketHandle();
            SocketHandle();
		};

        virtual ~SocketBase() = 0;
    };
    inline SocketBase::~SocketBase() {};
}

