#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h> 
#include <cstdio>
#include <cstdlib>
#include <netinet/in.h>
#include <arpa/inet.h>

int main() 
{
	int server_socket;
	server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) 
		{
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    std::cout << "Socket build =) " << std::endl;
    sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(6667);
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
    int bind_val = bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address));
    if (bind_val)
    {
        std::cerr << "bind error" << std::endl;
        exit(EXIT_FAILURE);
    }
    int list_val = listen(server_socket, 5); 
    if (list_val)
    {
        std::cerr << "listen error" << std::endl;
        exit(EXIT_FAILURE);
    }
    std::cout << "le serveur écoute" << std::endl;
    while(1)
    {
        int acc;
        socklen_t sock_len = sizeof(server_address);
        acc = accept(server_socket, (struct sockaddr *)&server_address, &sock_len);
        if(acc < 0)
        {
            std::cerr << "accept error" << std::endl;
            exit (EXIT_FAILURE);
        }
		char buff[1024];
		int rec_val = recv(acc, buff, sizeof(buff), 0);
		if(rec_val != -1)
            std::cout << "client message: "<< buff <<std::endl;
	}
    close(server_socket);
    return 0;
}