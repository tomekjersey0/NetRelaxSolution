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

		int Bind(const char* ip, uint16_t port);
		int Listen(int backlog);
		std::optional<ConnectedSocket> Accept() const;

	};
}



