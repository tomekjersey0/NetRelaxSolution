#include "Server.h"
#include "Util.h"

#include "SocketsLibrary.h"

#include <mutex>
#include <sstream>

void App::Server::clientLoop(std::shared_ptr<ClientData> client) {
	while (server.isRunning()) {
		std::string msg = client->Recv();
		if (msg.empty())
			break;

		if (isCommand(msg)) {
			if (!runCommand(client, msg)) {
				client->SendLine("Invalid command. Use /help for a list of commands.");
			}
		}
		else {
			
		}

		Util::printClientMsg(msg, client->username);
	}
}

bool App::Server::runCommand(std::shared_ptr<ClientData> client, const std::string& msg) {
	std::istringstream iss(msg);
	std::string cmd;
	iss >> cmd;

	if (commandsList.find(cmd) == commandsList.end()) {
		return false;
	}

	std::vector<std::string> args;
	std::string arg;
	while (iss >> arg) {
		args.push_back(arg);
	}

	commandsList.at(cmd).command(client, args);
	return true;
}

void App::Server::clientThreadHandler(std::shared_ptr<ClientData> client) {
	bool clientConnected = true;
	if (!client->SendLine("Welcome to EchoNet!"))
		clientConnected = false;

	/* Register User with a new Username */
	bool usernameTaken = false;
	while ((client->username.empty() || usernameTaken) && clientConnected) {
		if (!client->SendLine("Enter your username:")) {
			clientConnected = false;
			break;
		}

		client->username = client->Recv();
		if (client->username.empty()) {
			clientConnected = false;
			break;
		}

		Util::strip_ip(client->username);

		std::lock_guard<std::mutex> lock(clientMutex);
		if (clients.find(client->username) == clients.end()) {
			clients[client->username] = client;
			usernameTaken = false;
			Net::Info("User: '" + client->username + "' registered");
		}
		else {
			usernameTaken = true;
			if (!client->SendLine("Username already taken.")) {
				clientConnected = false;
				break;
			}
		}
	}

	if (clientConnected) {
		clientLoop(client);
	}

	// Remove client from clients map
	Net::Info("Client disconnected: " + client->username);
	{
		std::lock_guard<std::mutex> lock(clientMutex);
		if (clients.find(client->username) != clients.end()) {
			Net::Info("Removed user: '" + client->username + "'");
			clients.erase(client->username);
		}
	}

	// Signal that this thread has finished
	{
		std::lock_guard<std::mutex> lock(clientThreadsMutex);
		for (auto& thread : clientThreads) {
			if (thread.thread.get_id() == std::this_thread::get_id()) {
				thread.finished = true;
				break;
			}
		}
	}
}

void App::Server::clientCleanupThreadHandler() {
	while (server.isRunning()) {
		Net::sleep_ms(5000);

		std::vector<std::thread> threadsToJoin;

		{
			std::lock_guard<std::mutex> lock(clientThreadsMutex);
			for (auto it = clientThreads.begin(); it != clientThreads.end(); ) {
				if (it->finished) {
					if (it->thread.joinable()) {
						threadsToJoin.push_back(std::move(it->thread));
					}
					it = clientThreads.erase(it);
				}
				else {
					++it;
				}
			}
		}

		// Join outside of lock
		for (auto& t : threadsToJoin) {
			t.join();
		}
	}
}


void App::Server::serverInputThreadHandler() {
	std::string str;
	std::cin >> str;
	if (str == "quit") {
		std::lock_guard<std::mutex> lock(clientMutex);
		for (auto it = clients.begin(); it != clients.end(); ++it) {
			auto& [username, client] = *it;
			client->SendLine("[SERVER] Server has shut down.");
		}

		clients.clear();
		server.Stop();
	}
}

void App::Server::Run(const char* ip) {
	Run(ip, defaultPort);
}
void App::Server::Run(uint16_t port) {
	Run(defaultIp, port);
}
void App::Server::Run() {
	Run(defaultIp, defaultPort);
}

void App::Server::Run(const char* ip, uint16_t port) {
	server.Start(ip, port);
	std::thread inputThread(&App::Server::serverInputThreadHandler, this);
	std::thread cleanupThread(&App::Server::clientCleanupThreadHandler, this);
	serverThreads.push_back(std::move(inputThread));
	serverThreads.push_back(std::move(cleanupThread));
	while (server.isRunning()) {
		auto _clientSocket = server.Accept();
		// stop new clients from being 'onboarded' if they joined here and the server is stopped
		if (!server.isRunning()) break;
		if (_clientSocket) {
			Net::Info("New client connected!");
			auto& clientSocket = _clientSocket.value();

			auto newClient = std::make_shared<ClientData>();
			newClient->sock = std::move(clientSocket);

			std::thread clientThread(&App::Server::clientThreadHandler, this, newClient);

			ClientThread clientThreadContainer;
			clientThreadContainer.thread = std::move(clientThread);
			std::lock_guard<std::mutex> lock(clientThreadsMutex);
			clientThreads.push_back(std::move(clientThreadContainer));
		}
	}

	joinAllThreads();
}

void App::Server::joinAllThreads() {
	for (auto& thread : clientThreads) {
		if (thread.thread.joinable()) {
			thread.thread.join();
		}
	}
	for (auto& thread : serverThreads) {
		if (thread.joinable()) {
			thread.join();
		}
	}
}

App::Server::~Server() {
	try {
		server.Stop();
	}
	catch (const std::exception& e) {
		std::cerr << "Error stopping server: " << e.what() << std::endl;
	}

	joinAllThreads();
}

bool App::Server::ClientData::Send(const std::string& msg) const {
	int bytes = sock.Send(msg);
	if (bytes == Net::SOCKET_ERROR_VALUE) {
		return false;
	}
	return true;
}

bool App::Server::ClientData::SendLine(const std::string& msg) const {
	return Send(msg + '\n');
}

bool App::Server::ClientData::SendTo(const std::string& msg, const std::string& username) const {
	std::lock_guard<std::mutex> lock(clientMutex);
	if (clients.find(username) != clients.end()) {
		int bytes = (*clients[username]).sock.Send(msg);
		if (bytes == Net::SOCKET_ERROR_VALUE) {
			return false;
		}
		return true;
	}
	return false;
}

bool App::Server::ClientData::SendLineTo(const std::string& msg, const std::string& username) const {
	return SendTo(msg + '\n', username);
}


void App::Server::broadcastMessage(const std::string& msg, const std::string& clientUsername) {
	std::lock_guard<std::mutex> lock(clientMutex);
	for (auto& _c : clients) {
		auto& [username, c] = _c;
		if (username != clientUsername) {
			if (!c->Send(msg)) {
				// oh shit
			}
		}
	}
}

std::string App::Server::ClientData::_Recv() const {
	constexpr size_t kRecvBufferSize = 1028;
	std::string buffer(kRecvBufferSize, '\0');
	int bytes = sock.Recv(&buffer[0], buffer.size());

	if (bytes < 0) {
		// -1 means error
		Net::Error(Net::GetError::getLastErrorFullMessage());
		return "";
	}
	else if (bytes == 0) {
		// disonnected
		return "";
	}

	buffer.resize(bytes);

	return buffer;
}

std::string App::Server::ClientData::Recv() {
	while (true) {
		// If we already have a complete line buffered, return it immediately.
		size_t newlinePos = _buffer.find('\n');
		if (newlinePos != std::string::npos) {
			std::string line = _buffer.substr(0, newlinePos + 1);  // include '\n'
			_buffer.erase(0, newlinePos + 1);
			return line;
		}

		// Read more data.
		std::string recvOut = _Recv();
		if (recvOut.empty()) {
			// No more data from socket; return any leftover buffer (partial line).
			if (!_buffer.empty()) {
				std::string leftover = _buffer;
				_buffer.clear();
				return leftover;
			}
			return "";
		}

		_buffer += recvOut;
	}
}



bool App::Server::isCommand(const std::string& msg) {
	return !msg.empty() && msg[0] == '/';
}