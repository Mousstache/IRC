#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "Client.hpp"
#include "Server.hpp"

class Client;

class Channel {
public:
    Channel(std::string name, Client *client);
    Channel(Client *client, bool isAdmin);
    ~Channel();
    void addClient(Client *client);
    bool getRegister() const;
    std::vector<Client *> getClients() const;
    void setAdmin(std::string clientNickname);
    bool isAdmin(std::string clientNickname) const;
    std::string const &getName();
    void print() const;
    void setPassword(std::string password);
    std::string const &getPassword();
    bool neededPassword();


    private:
        std::string _name;
        bool _isRegistered;
        bool _needpassword;
        std::string _password;
        std::vector<Client *> _clients;
        std::map<std::string, bool> _admins;

};

#endif