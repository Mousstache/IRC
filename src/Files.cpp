#include <iostream>
#include <fstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

void sendFile(int clientSocket, const char* filename) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file) {
        std::cerr << "Erreur lors de l'ouverture du fichier: " << filename << std::endl;
        return;
    }
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    send(clientSocket, reinterpret_cast<char*>(&size), sizeof(size), 0);
    char buffer[1024];
    while (file.read(buffer, sizeof(buffer))) {
        send(clientSocket, buffer, file.gcount(), 0);
    }
    send(clientSocket, buffer, file.gcount(), 0);

    file.close();
}

void receiveFile(int clientSocket, const char* filename) {
    std::streamsize size;
    recv(clientSocket, reinterpret_cast<char*>(&size), sizeof(size), 0);

    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Erreur lors de la création du fichier: " << filename << std::endl;
        return;
    }

    char buffer[1024];
    std::streamsize bytesReceived = 0;
    while (bytesReceived < size) {
        int result = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (result <= 0) break; // Erreur ou fin de la transmission
        file.write(buffer, result);
        bytesReceived += result;
    }

    file.close();
}