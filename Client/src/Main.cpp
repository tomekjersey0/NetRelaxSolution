#include "SocketsLibrary.h"

/* Main file for ClientApp */

int main()
{
    try {
        Net::Client client;
    }
    catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}