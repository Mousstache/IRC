#include "Channel.hpp"

Channel::Channel(Client *client, bool isAdmin)
{
    addClient(client);
    _needpassword = false;
    if(isAdmin)
    {
        setAdmin(client->getNickname());
    }
    _isRegistered = true;
}

Channel::Channel(std::string name, Client *client)
{
    _name = name;
    _needpassword = false;
    addClient(client);
    setAdmin(client->getNickname());
    _isRegistered = true;
}

std::string const &Channel::getName()
{
    return (_name);
}

void Channel::addClient(Client *client) {
    _clients.push_back(client);
}

bool Channel::getRegister() const {
    return _isRegistered;
}

std::vector<Client *> Channel::getClients() const {
    return _clients;
}

void Channel::setAdmin(std::string clientNickname) {
    _admins[clientNickname] = true;
}

bool Channel::isAdmin(std::string clientNickname) const {
    return (_admins.find(clientNickname) != _admins.end());
}

void Channel::setPassword(std::string password)
{
    _password = password;
    _needpassword = true;
}
std::string const &Channel::getPassword()
{
    return(_password);
}

bool Channel::neededPassword()
{
    return(_needpassword);
}

void Channel::print() const
{
    std::cout << "Channel: " << _name << "\n";
    if (_needpassword == true)
    {
        std::cout << "Password =" << _password << std::endl;
    }
    std::cout << "Clients\t\tAdmins\n";
    size_t maxClients = _clients.size();
    size_t maxAdmins = _admins.size();
    size_t max = std::max(maxClients, maxAdmins);
    for (size_t i = 0; i < max; ++i) {
        if (i < maxClients) {
            std::cout << _clients[i]->getNickname() << "\t\t";
        } else {
            std::cout << "\t\t";
        }
        if (i < maxAdmins) {
            std::map<std::string, bool>::const_iterator it = _admins.begin();
            std::advance(it, i);
            std::cout << it->first;
        }
        std::cout << "\n";
    }
}
