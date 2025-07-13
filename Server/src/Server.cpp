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
	while ((client->username == "" || usernameTaken) && clientConnected) {
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
			client->username = client->username;
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

	// remove client from clients vector
	Net::Info("Client disconnected: "+ client->username);
	std::lock_guard<std::mutex> lock(clientMutex);
	if (clients.find(client->username) != clients.end()) {
		Net::Info("Removed user: '" + client->username + "'");
		clients.erase(client->username);
	}
}

void App::Server::clientCleanupThreadHandler() {
	while (server.isRunning()) {
		Net::sleep_ms(5000);
		std::lock_guard<std::mutex> lock(clientThreadsMutex);
		for (auto& thread : clientThreads) {
			if (thread.thread.joinable()) {
				thread.finished = true;
				thread.thread.join();
			}
		}

		for (auto it = clientThreads.begin(); it != clientThreads.end(); ) {
			if (it->finished) {
				it = clientThreads.erase(it);
			}
			else {
				++it;
			}
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

void App::Server::Run() {
	server.Start();
	std::thread inputThread(&App::Server::serverInputThreadHandler, this);
	std::thread cleanupThread(&App::Server::clientCleanupThreadHandler, this);
	serverThreads.emplace_back(std::move(inputThread));
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

std::string App::Server::ClientData::Recv() const {
	constexpr size_t kRecvBufferSize = 1028;
	std::string buffer(kRecvBufferSize, '\0');
	int bytes = sock.Recv(&buffer[0], buffer.size());

	if (bytes <= 0) {
		// connection closed or error
		Net::Error(Net::GetError::getLastErrorFullMessage());
		return "";
	}

	buffer.resize(bytes);
	return buffer;
}


bool App::Server::isCommand(const std::string& msg) {
	return !msg.empty() && msg[0] == '/';
}