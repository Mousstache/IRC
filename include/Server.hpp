#ifndef SERVER_HPP
#define SERVER_HPP

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
        int _socket;
        struct sockaddr_in _sin;
        int _bin;
        int _lis;
        int _port;
        std::string _password;
        bool channelExists(const std::string& name) const;
        void joinChannel(const std::string& channelName, const std::string& pass, Client* client, bool isAdmin);
        void set_id(std::string &str, std::vector<Client>::iterator it);
        bool parse(std::vector<Client>::iterator it, std::string buffer);
        Server(std::string port, std::string password);
        void exec_cmd(std::string const &command, std::string const &value, std::vector<Client>::iterator it);
        bool cmd_pars(std::vector<Client>::iterator it, std::string buffer);
        void serving();
        ~Server();
    private:
        std::map<std::string, Channel*> channels;
};

#endif