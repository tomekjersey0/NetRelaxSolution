#include "Server.h"
#include "Util.h"

#include <sstream>
#include <memory>

void App::Server::helpHandler(std::shared_ptr<ClientData> client, const std::vector<std::string>& args) {
	std::vector<std::string> full(commandsList.size());
	int i = 0;
	for (const auto& [cmd, info] : commandsList) {
		std::string argsString;
		for (const auto& a : info.args) {
			if (!argsString.empty()) argsString += " ";
			argsString += a;
		}
		full[i++] = cmd + " " + argsString;
	}

	size_t longest = Util::findLongsetStringLength(full);

	std::ostringstream iss;
	iss << "Available commands:" << std::endl;
	for (const auto& [cmd, info] : commandsList) {
		std::string padded = cmd;
		for (std::string arg : info.args) {
			padded += " " + arg;
		}
		padded.resize(longest + 1, ' ');
		iss << padded << " - " << info.description << std::endl;
	}
	client->Send(iss.str());
}

void App::Server::listHandler(std::shared_ptr<ClientData> client, const std::vector<std::string>& args) {
	std::ostringstream iss;
	for (auto it = clients.begin(); it != clients.end(); ++it) {
 		auto& [username, data] = *it;
		iss << username;
		if (username == client->username) {
			iss << " (You)";
		}
		iss << std::endl;
	}
	client->Send(iss.str());
}

void App::Server::requestHandler(std::shared_ptr<ClientData> client, const std::vector<std::string>& args) {
	if (args.size() > 1) {
		client->SendLine("Invalid syntax. /request takes only one argument.");
		return;
	}
	if (args.size() == 0) {
		client->SendLine("Invalid syntax. Missing username specifier.");
		return;
	}
	
	std::string target = args[0];
	if (target == client->username) {
		client->SendLine("Invalid argument. Can't request to chat with yourself, sorry.");
		return;
	}
	
	// send request
	{
		std::lock_guard<std::mutex> lock(clientMutex);
		if (clients.find(target) != clients.end()) {
			if (clients.find(client->pending_request_to) != clients.end()) {
				if (target != client->pending_request_to) {
					std::ostringstream iss;
					iss << "[SERVER] '" << client->username << "' has cancelled their chat request with you.";
					clients[client->pending_request_to]->SendLine(iss.str());
				}
			}

			client->SendLine("Sent chat request to: " + target);
			client->pending_request_to = target;

			std::ostringstream iss;
			iss << "[SERVER] '" << client->username << "' has requested to chat with you. \n[SERVER] /accept " << client->username << " to connect.";
			clients[target]->SendLine(iss.str());

		}
		else {
			client->SendLine("User: " + target + " does not exist.");
		}
	}
}

void App::Server::acceptHandler(std::shared_ptr<ClientData> client, const std::vector<std::string>& args) {

}


void App::Server::rejectHandler(std::shared_ptr<ClientData> client, const std::vector<std::string>& args) {

}
