/* Main file for ServerApp */

#include "Server.h"

static void ServerAppMain(int argc, char* argv[]) {
    if (argc > 3) {
        std::cerr << "Format: ServerApp <ip> <port>" << std::endl;
        exit(1);
    }
    App::Server serverApp;
    if (argc == 3) {
        char* ip = argv[1];
        try {
            uint16_t port = static_cast<uint16_t>(std::stoi(argv[2]));
            serverApp.Run(ip, port);
        }
        catch (const std::invalid_argument&) {
            std::cerr << "Invalid port: must be a number.\n";
            exit(1);
        }
        catch (const std::out_of_range&) {
            std::cerr << "Invalid port: number out of range.\n";
            exit(1);
        }

    }
    else if (argc == 2) {
        char* ip = argv[1];
        serverApp.Run(ip);
    }
    else {
        serverApp.Run();
    }
}

int main(int argc, char* argv[])
{
    try {
        ServerAppMain(argc, argv);
    }
    catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        std::cout << "Failure" << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}