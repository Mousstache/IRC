#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#pragma once
#include "Client.hpp"
#include "Server.hpp"

class Client;

class Channel {
    public:
        ~Channel();
        Channel(Client *client,std::string channelname,std::string password);
        Channel(std::string channelname, std::string password);
        std::string getName();
        std::string getPassword();
        std::string getTopic(){return (_topic);};
        bool        getNeed();
        void        addClients(Client *clients);
        void        addAdmins(Client *admins);
        void        rmClients(std::string clientname);
        void        rmAdmins(std::string adminname);
        void        setTopic(std::string top){_topic = top;};
        void        print();

    private:
        std::string _name;
        std::string _password;
        bool        _needpass;
        std::string _topic;
        std::map<std::string, Client *> _clients;
        std::map<std::string, Client *> _admins;

};

#endif