#include "pch.h"

#include <iostream>
#include <string>

#include "Server.h"

Net::Server::~Server() {
	Stop();
}

void Net::Server::Stop() {
	running = false;
	listener.s.CloseSocket();
}

void Net::Server::Start() {
	Start(defaultIp, defaultPort);
}

void Net::Server::Start(const char* ip) {
	Start(ip, defaultPort);
}

void Net::Server::Start(const uint16_t port) {
	Start(defaultIp, port);
}

// used defined port and ip for starting of server
void Net::Server::Start(const char* ip, uint16_t port) {
	running = true;

	if (listener.s.socket != Net::INVALID_SOCKET_VALUE) {
		Error("Server already started, call Stop() before starting again");
		throw std::runtime_error("Server already started");
	}

	if (listener.s.CreateSocket() == Net::INVALID_SOCKET_VALUE) {
		Error("Failed to create socket");
		throw std::runtime_error("Failed to initialise socket");
	}
	Info("Socket created successfully");

	if (listener.Bind(ip, port) == SOCKET_ERROR_VALUE) {
		std::cerr << Net::GetError::getLastErrorFullMessage() << std::endl;
		Stop();
		throw std::runtime_error("Failed to bind to socket");
	}
	Info("Socket bound successfully");

	if (listener.Listen(SOMAXCONN) == SOCKET_ERROR_VALUE) {
		std::cerr << Net::GetError::getLastErrorFullMessage() << std::endl;
		Stop();
		throw std::runtime_error("Failed to listen on socket");
	}
	Info("Server started on "+std::string(ip)+":"+std::to_string(port));
}