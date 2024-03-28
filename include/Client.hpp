#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Server.hpp"
#include <iostream>

class Client
{
    public:
        int socket_fd;
        struct sockaddr_in address;
        Client(int fd, struct sockaddr_in addr);
        ~Client();
        bool const & getRegister();
        std::string const & getNickname();
        std::string const & getUsername();
        std::string const & getPassword();
        bool setAccount();
        void setRegister(bool reg);
        void setNickname(std::string str);
        void setUsername(std::string str);
        void setPassword(std::string str);
        int _check;
    private:
        bool _register;
        std::string _nickname;
        std::string _username;
        std::string _password;
};

#endif