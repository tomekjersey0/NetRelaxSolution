#pragma once

namespace Net {
	class ConnectedSocket : public SocketBase
	{
	public:
        SocketHandle connection;
   
        ConnectedSocket() {
            connection = SocketHandle();
        }

        // Cannot copy a ConnectedSocket
        ConnectedSocket(const ConnectedSocket&) = delete;
        ConnectedSocket& operator=(const ConnectedSocket&) = delete;

        ConnectedSocket(ConnectedSocket&& other) noexcept
            : connection(std::move(other.connection)) {
            // Invalidate other's connection.socket here if needed
            other.connection.socket = INVALID_SOCKET_VALUE;
        }

        ConnectedSocket& operator=(ConnectedSocket&& other) noexcept {
            if (this != &other) {
                // Close this->connection if valid
                // Move other's connection
                connection = std::move(other.connection);
                other.connection.socket = INVALID_SOCKET_VALUE;
            }
            return *this;
        }

		int Send(const std::string& msg) const {
			return send(connection.socket, msg.c_str(), msg.size(), 0);
		}
		int Recv(char* buffer, size_t size) const {
			return recv(connection.socket, buffer, size, 0);
		}
	};
}