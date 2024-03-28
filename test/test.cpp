#include <cstdlib>
#include <cstring>
#include <climits>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <signal.h>
#include <poll.h>
#include <fcntl.h>
#include <stdio.h>


bool ft_check_port_value(int port)
{
    if (port < 1028 || port > 65535)
    {
        std::cout << "bad value port" << std::endl;
        return (false);
    }
    return (true);
}

bool ft_check_port(int argc, char **argv)
{
    if(argc != 2)
    {
        std::cout << "./ircserv <port> <password>" << std::endl;
        return (false);
    }
    if(!*argv[1])
    {
        std::cout << "./ircserv <port> <password>" << std::endl;
        return (false);
    }
    for (size_t i = 0; argv[1][i]; i++)
    {
        if (argv[1][i] < '0' || argv[1][i] > '9' || i > 5)
        {
            std::cout << "you know the problem" << std::endl;
            return(false);
        }
    }
    
    int i = std::atoi(argv[1]);
    if (!ft_check_port_value(i))
    {
        return (false);
    }
    return (true);
}

void handle_sigint(int sig)
{
    (void)sig;
    std::cout << "Arrêt du serveur...\n";
    exit(0); // Termine le programme proprement
}

int main(int argc, char **argv)
{
    if (!ft_check_port(argc, argv))
    {
        return (1);
    }
    // Création du socket du serveur
    int port = std::atoi(argv[1]);
    signal(SIGINT, handle_sigint);
    signal(SIGTSTP, handle_sigint);
    char buffer[1024] = {0};
    (void)buffer;
    while(std::cin) // Ctrl+D fera terminer la boucle
    {
        int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (serverSocket == -1) {
            perror("socket");
        return (1);
        }

        sockaddr_in serverAddress;
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(port);
        serverAddress.sin_addr.s_addr = INADDR_ANY;
        int yes = 1;
        if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0)
        {
            perror("setsockopt");
        return (1);
        }
        if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
            perror("bind");
        return (1);
        }

        if (listen(serverSocket, 5) == -1) {
            perror("listen");
        return (1);
        }

        while (std::cin) {
            sockaddr_in clientAddress;
            socklen_t clientAddressLength = sizeof(clientAddress);
            int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddressLength);
            if (clientSocket == -1) {
                perror("accept");
                continue; // Continue to listen for new connections
            }

            // Configuration du socket client en mode non bloquant
            int flags = fcntl(clientSocket, F_GETFL, 0);
            if (flags == -1)
            {
                perror("fcntl");
                close(clientSocket);
                continue;
            }
            if (fcntl(clientSocket, F_SETFL, flags | O_NONBLOCK) == -1) {
                perror("fcntl");
                close(clientSocket);
                continue;
            }

            // Utilisation de poll pour gérer les opérations non bloquantes
            struct pollfd fds[1];
            fds[0].fd = clientSocket;
            fds[0].events = POLLIN;

            while (true)
            {
                int ret = poll(fds, 1, -1);
                if (ret > 0 && (fds[0].revents & POLLIN)) {
                    char buffer[1024] = {0};
                    (void)buffer;
                    ssize_t bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
                    if (bytesReceived > 0)
                    {
                        std::cout << "Message du client: " << buffer;
                        close(clientSocket);
                        break;
                    }
                    if (bytesReceived == 0)
                    {
                        // Client déconnecté
                        std::cout << bytesReceived << std::endl;
                        close(clientSocket);
                        break;
                    }
                    if (bytesReceived == -1)
                    {
                        // Erreur lors de la réception
                        std::cout << bytesReceived << std::endl;
                        close(clientSocket);
                        perror("recv");
                        break;
                    }
                }
            }
            // Fermeture du socket
            close(clientSocket);
        }
        close(serverSocket);
    }
    return 0;
}