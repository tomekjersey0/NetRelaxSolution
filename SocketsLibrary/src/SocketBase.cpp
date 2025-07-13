#include "pch.h"

#include <iostream>

#include "GetError.h"
#include "SocketBase.h"


Net::socket_t Net::SocketBase::SocketHandle::CreateSocket() {
    socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (socket == Net::INVALID_SOCKET_VALUE) {
        std::cerr << Net::GetError::getLastErrorFullMessage() << std::endl;
    }
    return socket;
}

void Net::SocketBase::SocketHandle::CloseSocket() noexcept {
    if (socket != INVALID_SOCKET_VALUE) {
        if (Net::CloseSocket(socket) == Net::SOCKET_ERROR_VALUE) {
            std::cerr << Net::GetError::getLastErrorFullMessage() << std::endl;
        }
        socket = INVALID_SOCKET_VALUE;
    }
}

Net::SocketBase::SocketHandle::~SocketHandle() {
    CloseSocket();
}

Net::SocketBase::SocketHandle::SocketHandle() {
    socket = INVALID_SOCKET_VALUE;
    addr = sockaddr_in{};
}