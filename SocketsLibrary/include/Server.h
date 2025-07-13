#pragma once

#include "ListenerSocket.h"

#include <optional>

namespace Net {
    class Server
    {
    private:
        const char* defaultIp = "127.0.0.1";
        const uint16_t defaultPort = 55555;

        std::atomic<bool> running{ true };
        Program program;
        ListenerSocket listener;

    public:
        struct ClientSocket {
            socket_t socket;
            sockaddr_in addr;
        };

        int Listen() {
            return listener.Listen(SOMAXCONN);
        }

        int Bind(const char* ip, uint16_t port) {
            return listener.Bind(ip, port);
        }

        std::optional<ConnectedSocket> Accept() {
            return listener.Accept();
        }

        bool isRunning() const { return running.load(); }

        ~Server();
        // Creates a new socket, bind to a given ip address, and starts to listen on it
        void Start();
        void Start(const char* ip);
        void Start(uint16_t port);
        void Start(const char* ip, uint16_t port);



        // Close the listener socket
        void Stop();
    };

}