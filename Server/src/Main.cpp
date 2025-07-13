/* Main file for ServerApp */

#include "Server.h"

static void ServerAppMain() {
    App::Server serverApp;
	serverApp.Run();
}

int main()
{
    try {
        ServerAppMain();
    }
    catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        std::cout << "Failure" << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}