#ifndef CLIENT_HPP
#define CLIENT_HPP
#pragma once

#include "Server.hpp"
#include "Channel.hpp"
#include <iostream>
#include <map>

class Channel;

class Client
{
    public:
        Client();
        Client(int fd, struct sockaddr_in addr);
        ~Client();
        std::string getPass();
        std::string getUsername();
        std::string getNickname();
        std::string getRealname();
        int getSocket();
        Client *getClient();
        std::map<std::string ,Channel *> getUserChannel();
        void addChannel(std::string channelname, std::string password);
        void rmChannel(std::string channelname);
        bool alreadylog(std::string channelname);
        bool getRegister();
        void setRegister(bool value);
        void setPassword(std::string pass);
        void setNickname(std::string nick);
        void setUsername(std::string username);
        struct sockaddr_in _address;
        void checkplus();
        int getCheck();
        int _socket_fd;

    private:
        std::string _pass;
        std::string _username;
        std::string _nickname;
        std::string _realname;
        bool        _register;
        int  _check;
        // std::map<std::string, Channel *> _userchannel;
};

#endif