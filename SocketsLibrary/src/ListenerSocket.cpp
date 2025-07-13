#include "pch.h"

#include "ListenerSocket.h"


int Net::ListenerSocket::Listen(int backlog) const {
	return listen(s.socket, backlog);
}

int Net::ListenerSocket::Bind(const char* ip, uint16_t port) const {
	sockaddr_in addr{};
	addr.sin_port = htons(port);
	addr.sin_family = AF_INET;

	int result = inet_pton(AF_INET, ip, &(addr.sin_addr));
	if (result == 1) {
		return bind(s.socket, reinterpret_cast<const sockaddr*>(&addr), sizeof(addr));
	}
	if (result == 0) {
		std::string msg = "Invalid ip address passed to inet_pton: " + std::string(ip);
		throw std::runtime_error(msg);
	}
	if (result == -1) {
		std::cout << Net::GetError::getLastErrorFullMessage() << std::endl;
		throw std::runtime_error("Something went wrong in inet_pton");
	}
}

std::optional<Net::ConnectedSocket> Net::ListenerSocket::Accept() const {
	ConnectedSocket client;
	int addr_len = sizeof(client.connection.addr);

	client.connection.socket = accept(s.socket, reinterpret_cast<sockaddr*>(&client.connection.addr), &addr_len);
	if (client.connection.socket == Net::INVALID_SOCKET_VALUE) {
		int err = Net::GetError::getLastErrorCode();
		std::string msg = Net::GetError::getLastErrorFullMessage();

		// Recoverable: don't throw
#ifdef _WIN32
		if (err == WSAEWOULDBLOCK || err == WSAEINTR)
#else
		if (err == EAGAIN || err == EWOULDBLOCK || err == EINTR)
#endif
		{
			Info("Transient accept() failure: " + msg);
			return std::nullopt;
		}

		// Fatal: unrecoverable accept failure
		Error("Fatal accept() failure: " + msg);
		throw std::runtime_error("Accept() failed permanently: " + msg);
	}
	return client;
}