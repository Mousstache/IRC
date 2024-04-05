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
        std::string getTopic();
        void        setTopic(const std::string &newTopic);
        bool        getNeed();
        void        addClients(Client *clients);
        void        addAdmins(Client *admins);
        void        rmClients(std::string clientname);
        void        rmAdmins(std::string adminname);
        void        chanmsg(std::string msg);
        void        print();
        std::map<std::string, Client *> getAdmins();
        std::map<std::string, Client *> getClients();

    private:
        std::string _name;
        std::string _password;
        std::string _topic;
        bool        _needpass;
        std::map<std::string, Client *> _clients;
        std::map<std::string, Client *> _admins;
};

#endif