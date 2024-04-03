#include "Channel.hpp"

Channel::~Channel()
{
    _clients.clear();
    _admins.clear();
}

std::map<std::string, Client *> Channel::getAdmins()
{
    return (_admins);
}
std::map<std::string, Client *> Channel::getClients()
{
    return(_clients);
}

Channel::Channel(Client *client, std::string channelname, std::string password)
{
    _name = channelname;
    _password = password;
    _needpass = true;
    _clients[client->getUsername()] = client;
    _admins[client->getUsername()] = client;
}

bool Channel::clientExist(std::string name)
{
    if(_clients.find(name) != _clients.end())
        return (true);
    return (false);
}

Channel::Channel(std::string channelname, std::string password)
{
    _name = channelname;
    _password = password;
    _needpass = false;
}
std::string Channel::getName()
{
    return(_name);
}
std::string Channel::getPassword()
{
    return(_password);
}
bool       Channel::getNeed()
{
    return(_needpass);
}
void       Channel::addClients(Client *clients)
{
    if(_clients.find(clients->getUsername()) == _clients.end())
    {
        _clients[clients->getUsername()] = clients;
    }
}

void       Channel::addAdmins(Client *admins)
{
    if(_admins.find(admins->getUsername()) == _admins.end())
    {
        _admins[admins->getUsername()] = admins;
    }
}
void       Channel::rmClients(std::string clientname)
{
    std::map<std::string , Client *>::iterator it = _clients.find(clientname);
    if (it != _clients.end())
    {
        _clients.erase(it);
    }
}
void       Channel::rmAdmins(std::string adminname)
{
    std::map<std::string , Client *>::iterator it = _admins.find(adminname);
    if (it != _admins.end())
    {
        _admins.erase(it);
    }
}

void Channel::chanmsg(std::string msg)
{
    std::cout << _clients.size() << std::endl;
    for (std::map<std::string, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
        Client* client = it->second;
        std::cout << "cooooool" << msg << std::endl;
        int clientSocket = client->getSocket(); // Obtenez le socket du client
        // Envoyez le message au client
        send(clientSocket, msg.c_str(), msg.size(), 0);
    }
}

void Channel::print() {
    std::cout << "Nom du canal: " << _name << std::endl;
    std::cout << "Mot de passe: " << (_needpass ? _password : "Pas de mot de passe") << std::endl;
    std::cout << "Clients: ";
    std::map<std::string, Client *>::iterator it;
    for (it = _clients.begin(); it != _clients.end(); ++it) {
        std::cout << it->first << " ";
    }
    std::cout << std::endl;
    std::cout << "Admins: ";
    for (it = _admins.begin(); it != _admins.end(); ++it) {
        std::cout << it->first << " ";
    }
    std::cout << std::endl;
}