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
        void        setPassword(const std::string &newPassword);
        void        setTopic(const std::string &newTopic);
        bool        getInvitation();
        void        setInvitation(bool invitation);
        void        setPass(bool pass);
        void        setAdmintopic(bool admintopic);
        bool        getAdmintopic();
        void        setLimituser(bool limit, int nblimit);
        int         getLimituser();
        bool        getNeed();
        void        addClients(Client *clients);
        void        addAdmins(Client *admins);
        void        rmClients(std::string clientname);
        void        rmAdmins(std::string adminname);
        void        print();
        void        chanmsg(std::string msg, std::string user);
        void        addInvits(Client *invit);
        std::map<std::string, Client *> getInvits();
        void        rmInvit(std::string invitname);
        bool        clientExist(std::string exist);
        Client*     getInvitUser(std::string const & nameclient);
        Client*     getIencli(std::string const & nameclient);
        Client*     getAdminUser(std::string const & nameclient);
        bool        isInvited(const std::string& client);
        std::string client_list();


        std::map<std::string, Client *> getAdmins();
        std::map<std::string, Client *> getClients();
        bool        getLimit();
        long int time_chan;
        long int time_topic;

    private:
        std::string _name;
        std::string _password;
        std::string _topic;
        bool        _needpass;
        bool        _needinvitation;
        bool        _admintopic;
        bool        _limitclients;
        int         _limitusers;
        std::map<std::string, Client *> _clients;
        std::map<std::string, Client *> _invits;
        std::map<std::string, Client *> _admins;
};

#endif