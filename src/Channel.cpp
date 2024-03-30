#include "Channel.hpp"

Channel::~Channel()
{

}
Channel::Channel(Client *client, std::string channelname, std::string password)
{
    _name = channelname;
    _password = password;
    _needpass = true;
    _clients[client->getUsername()] = client;
    _admins[client->getUsername()] = client;
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
        delete it->second;
        _clients.erase(it);
    }
}
void       Channel::rmAdmins(std::string adminname)
{
    std::map<std::string , Client *>::iterator it = _admins.find(adminname);
    if (it != _admins.end())
    {
        delete it->second;
        _admins.erase(it);
    }
}
