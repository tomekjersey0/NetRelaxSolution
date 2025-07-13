#pragma once

#include "SocketBase.h"
#include "NetSocket.h"
#include "ConnectedSocket.h"

#include <optional>

namespace Net {
	class ListenerSocket : public SocketBase
	{
	public:
		SocketHandle s;

		ListenerSocket() {
			s = SocketHandle();
		}

		int Bind(const char* ip, uint16_t port) const;
		int Listen(int backlog) const;
		std::optional<ConnectedSocket> Accept() const;

	};
}



