#pragma once

#include "SocketsLibrary.h"
#include <vector>
#include <thread>
#include <mutex>
#include <unordered_map>
#include <memory>

namespace App
{
	class Server {
	private:
		const char* defaultIp = "127.0.0.1";
		const uint16_t defaultPort = 55555;

		static inline std::mutex clientMutex;
		static inline std::mutex clientThreadsMutex;

		struct ClientThread {
			std::thread thread;
			bool finished;

			ClientThread() {
				finished = false;
			}

			ClientThread(const ClientThread&) = delete;
			ClientThread& operator=(const ClientThread&) = delete;

			ClientThread(ClientThread&&) noexcept = default;
			ClientThread& operator=(ClientThread&&) noexcept = default;
		};

		struct ClientData {
			std::string username;
			Net::ConnectedSocket sock;
			std::string pending_request_to;
			std::string chatting_with;

			ClientData() {
				username = "";
				sock = Net::ConnectedSocket();
				pending_request_to = "";
				chatting_with = "";
			}

			bool Send(const std::string& msg) const;
			bool SendLine(const std::string& msg) const;
			bool SendTo(const std::string& msg, const std::string& username) const;
			bool SendLineTo(const std::string& msg, const std::string& username) const;
			std::string Recv() const;

		};
		void broadcastMessage(const std::string& msg, const std::string& clientUsername);

		static inline Net::Server server;
		static inline std::vector<std::thread> serverThreads;
		static inline std::unordered_map<std::string, std::shared_ptr<ClientData>> clients;
		static inline std::vector<ClientThread> clientThreads;

		void clientLoop(std::shared_ptr<ClientData> client);
		void clientThreadHandler(std::shared_ptr<ClientData> client);
		void clientCleanupThreadHandler();
		void joinAllThreads();

		void serverInputThreadHandler();

		using CommandHandler = void (*)(std::shared_ptr<ClientData>, const std::vector<std::string>&);
		struct Command {
			CommandHandler command;
			std::vector<std::string> args;
			std::string description;

			Command() = default;

			Command(CommandHandler cmd, const std::string& desc)
				: command(cmd), args(), description(desc) {
			}

			Command(CommandHandler cmd, std::vector<std::string> arguments, const std::string& desc)
				: command(cmd), args(std::move(arguments)), description(desc) {
			}
		};


		/* Client commands */
		static void helpHandler(std::shared_ptr<ClientData> client, const std::vector<std::string>& args);
		static void listHandler(std::shared_ptr<ClientData> client, const std::vector<std::string>& args);
		static void requestHandler(std::shared_ptr<ClientData> client, const std::vector<std::string>& args);
		static void acceptHandler(std::shared_ptr<ClientData> client, const std::vector<std::string>& args);
		static void rejectHandler(std::shared_ptr<ClientData> client, const std::vector<std::string>& args);


		bool isCommand(const std::string& msg);
		bool runCommand(std::shared_ptr<ClientData> client, const std::string& msg);
		static const inline std::unordered_map<std::string, Command> commandsList = {
			{"/help", Command(helpHandler, "show available commands")},
			{"/list", Command(listHandler, "list currently online users")},
			{"/request", Command(requestHandler, {"username"}, "ask a user if they want to chat")},
			{"/accept", Command(acceptHandler, {"username"}, "accept an incoming chat request")},
			{"/reject", Command(rejectHandler, {"username"}, "reject an incoming chat request")}
		};
	public:
		~Server();
		// Run the server, accepting clients and handling them
		void Run();
		void Run(const char* ip);
		void Run(uint16_t port);
		void Run(const char* ip, uint16_t port);
	};
}