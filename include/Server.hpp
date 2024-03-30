#ifndef SERVER_HPP
#define SERVER_HPP
#pragma once

#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h> 
#include <cstdio>
#include <cstdlib>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <map>
#include <string>
#include <iostream>
#include <vector>
#include <sstream>
#include <memory>
#include "Client.hpp"
#include "Channel.hpp"

class Client;
class Channel;

class Server {
    public:
        Server(std::string port, std::string password);
        Server(int port);
        ~Server();
        void serving();
        void startListen();
        void serve();
        std::string getPassword();
        void printserv();
        Client *getClient(int socket);
        Channel *getChannel(std::string channelname);
        void    joinChannel(Client *client, std::string channelname);
        void exitWithError(std::string errorMessage);
        void log(const std::string& message);
        Client *parse(Client *client, std::string buffer);

        void set_id(std::string str, Client *client);
        void exec_cmd(std::string const &command, std::string const &value, Client *client);
        bool cmd_pars(Client *client,std::string buffer);
        void joinChannel(Client *client, std::string channelname, std::string password);


    private:
        std::string _password;
        int _socket;
        struct sockaddr_in _sin;
        int _bin;
        int _lis;
        int _port;
        void handleNewConnection();
        void handleMessage(int client_socket, sockaddr_in newSockAddr);
        void handleDisconnection(int client_socket);
        std::map<std::string, Channel *> _channel;
        std::map<int, Client *>  _client;
};

#endif