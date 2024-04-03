#include "Client.hpp"
#include "Server.hpp"
#include <csignal>
#include <stdio.h>
#include <csignal>
#include <csetjmp>
#include <signal.h>
#include <unistd.h>

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


// jmp_buf gBuffer;

// void handle_sigint(int sig) {
//     longjmp(gBuffer, sig);
// }

// int main(int argc, char **argv) {
//     if (argc != 3) {
//         std::cerr << "Usage: port" << std::endl;
//         exit(0);
//     }
//     int sig = setjmp(gBuffer);
//     if (sig == 0) {
//         signal(SIGINT, handle_sigint);
//         try {
//              Server server(argv[1], argv[2]);
//              server.serving();
//         } catch (const std::exception& e) {
//             std::cerr << "Caught exception: " << e.what() << std::endl;

//             return 1;
//         } catch (...) {
//             std::cerr << "Caught unknown exception." << std::endl;
//             return 1;
//         }
//     } else {
//         std::cerr << "\n Signal reçu. Arrêt du serveur..." << std::endl;
//         // server.~Server();
//         return 1;
//     }

//     return 0;
// }

bool server_off = false;



// Fonction de gestion du signal SIGINT
void handle_sigint(int sig, siginfo_t *info, void *context) {
    (void)info;
    (void)context;
    if(sig)
    {
        server_off = true;
        std::cerr << "\n Signal reçu. Arrêt du serveur..." << std::endl;
    }
}

int main(int argc, char **argv) {
    if (argc != 3) {
        std::cerr << "Usage: port" << std::endl;
        exit(0);
    }

    // Configuration de la structure sigaction
    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO; // Utilisez SA_SIGINFO pour passer des informations supplémentaires au gestionnaire
    sa.sa_sigaction = handle_sigint; // Pointeur vers la fonction de gestion du signal
    sigemptyset(&sa.sa_mask); // Initialise le masque de signaux à vide

    // Installation du gestionnaire de signal pour SIGINT
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        if(SIGINT)
        {
            perror("sigaction");
            return 1;
        }
    }

    try {
        Server server(argv[1], argv[2]);
        server.serving();
    } catch (const std::exception& e) {
        std::cerr << "Caught exception: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Caught unknown exception." << std::endl;
    }

    return 0;
}
