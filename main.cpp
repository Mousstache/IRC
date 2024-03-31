#include "Client.hpp"
#include "Server.hpp"


// int main(int argc,char **argv)
// {
//     if (argc != 3|| !*argv[1] || !*argv[2]) {
//         std::cerr << "Usage: " << argv[0] << " <port>" << " <password> " << std::endl;
//         return 1;
//     }
//     Server server(argv[1], argv[2]);
//     server.serving();

//     return 0;
// }

// int main(int argc, char *argv[]) {
//     if (argc != 2) {
//         std::cerr << "Usage: port" << std::endl;
//         exit(0);
//     }
//     int port = atoi(argv[1]);
//     Server server(port);
//     server.startListen();
//     server.serve();
//     return 0;
// }

int main(int argc, char **argv) {
    if (argc != 3) {
        std::cerr << "Usage: port" << std::endl;
        exit(0);
    }
    try {
        // Initialize your server here
        Server server(argv[1], argv[2]);
        server.serving();
    } catch (const std::exception& e) {
        std::cerr << "Caught exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Caught unknown exception." << std::endl;
        return 1;
    }
    return 0;
}