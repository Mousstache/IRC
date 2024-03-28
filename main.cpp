#include "Client.hpp"
#include "Server.hpp"


int main(int argc,char **argv)
{
    if (argc != 3|| !*argv[1] || !*argv[2]) {
        std::cerr << "Usage: " << argv[0] << " <port>" << " <password> " << std::endl;
        return 1;
    }
    Server server(argv[1], argv[2]);
    server.serving();

    return 0;
}