#include "Channel.hpp"

Channel::~Channel()
{
    _clients.clear();
    _admins.clear();
    _invits.clear();
}

std::map<std::string, Client *> Channel::getAdmins()
{
    return (_admins);
}
std::map<std::string, Client *> Channel::getClients()
{
    return(_clients);
}

std::map<std::string, Client *> Channel::getInvits()
{
    return(_invits);
}

Channel::Channel(Client *client, std::string channelname, std::string password)
{
    _name = channelname;
    _password = password;
    _needpass = true;
    _clients[client->getNickname()] = client;
    _admins[client->getNickname()] = client;
    _needinvitation = false;
    _admintopic = false;
    _limitclients = false;
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

std::string Channel::getTopic()
{
    return (_topic);
}

void    Channel::setPassword(const std::string &newpassword)
{
    _password = newpassword;
}

void    Channel::setTopic(const std::string &newTopic)
{
    _topic = newTopic;
}

bool    Channel::getInvitation()
{
    return (_needinvitation);
}

void    Channel::setInvitation(bool invitation)
{
    _needinvitation = invitation;
}

bool        Channel::getAdmintopic()
{
    return (_admintopic);
}

void    Channel::setAdmintopic(bool admintopic)
{
    _admintopic = admintopic;
}

void    Channel::setPass(bool pass)
{
    _needpass = pass;
}

int Channel::getLimituser()
{
    return (_limitusers);
}

void    Channel::setLimituser(bool limit, int nblimit)
{
    _limitusers = nblimit;
    _limitclients = limit;
}

bool       Channel::getNeed()
{
    return(_needpass);
}
void       Channel::addClients(Client *clients)
{
    if(_clients.find(clients->getNickname()) == _clients.end())
    {
        _clients[clients->getNickname()] = clients;
    }
}

std::string Channel::client_list()
{
	std::string list;
	for (std::map<std::string, Client *>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
        if (_admins.find(it->second->getNickname()) != _admins.end())
		    list += " @" + it->second->getNickname();
        else
		    list += " " + it->second->getNickname();
	}
	return (list);
}

void       Channel::addAdmins(Client *admins)
{
    if(_admins.find(admins->getNickname()) == _admins.end())
    {
        _admins[admins->getNickname()] = admins;
    }
}

void       Channel::addInvits(Client *invit)
{
    if(_invits.find(invit->getNickname()) == _invits.end())
    {
        _invits[invit->getNickname()] = invit;
    }
}

void       Channel::rmClients(std::string clientname)
{
    std::map<std::string , Client *>::iterator it = _clients.find(clientname);
    if (it != _clients.end())
        _clients.erase(it);
}
void       Channel::rmAdmins(std::string adminname)
{
    std::map<std::string , Client *>::iterator it = _admins.find(adminname);
    if (it != _admins.end())
        _admins.erase(it);
}

void       Channel::rmInvit(std::string invitname)
{
    std::map<std::string , Client *>::iterator it = _invits.find(invitname);
    if (it != _invits.end())
        _invits.erase(it);
}

Client* Channel::getInvitUser(std::string const & nameclient)
{
    Client *tmp = NULL;
    if (_invits.find(nameclient) != _invits.end())
        tmp = _invits[nameclient];
    return (tmp);
}

Client* Channel::getIencli(std::string const & nameclient)
{
    Client *tmp = NULL;
    if (_clients.find(nameclient) != _clients.end())
        tmp = _clients[nameclient];
    return (tmp);
}

Client* Channel::getAdminUser(std::string const & nameclient)
{
    Client *tmp = NULL;
    if (_admins.find(nameclient) != _admins.end())
        tmp = _admins[nameclient];
    return (tmp);
}

bool Channel::clientExist(std::string exist)
{
    std::map<std::string, Client *>::iterator it =_clients.find(exist);
    if (it != _clients.end())
        return (true);
    return (false);
}

bool    Channel::isInvited(const std::string& client)
{
    if (_invits.find(client) != _invits.end())
    {
        if (getInvitation())
                return (true);
    }
    return (false);
}

bool    Channel::getLimit()
{
    return (_limitclients);
}

void Channel::chanmsg(std::string msg, std::string user)
{
    std::cout << "taille du chan chanmsg : " << _clients.size() << std::endl;
    std::string to_send;
    // std::string sock_test;
    // for (std::map<std::string, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
    //     Client* client = it->second;
    //     int clientSocket = client->getSocket();
	// 	to_send = RPL_PRIVMSG_CHANNEL(client->getNickname(), this->getName(), msg.c_str());
    //     std::cout << "le message : " << to_send << std::endl;
    //     send(clientSocket, msg.c_str(), msg.size(), 0);
    // }

    for (std::map<std::string, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
		Client* client = it->second;
		std::string envoyeur = client->getNickname();
        std::cout << "envoyeur ===" << envoyeur << " receveur ===" << it->second->getNickname() << std::endl;
		if ( envoyeur != user)
		{
            // std::cout << "fd envoyeur  == " << client->getNickname() << " fd client receveur == " << user << std::endl;
			// std::string to_send = RPL_PRIVMSG_CHANNEL_CMP(client->getNickname(), this->getName(), msg.c_str());
			// std::cout << "le message : " << to_send << std::endl;
			send(client->getSocket(), msg.c_str(), msg.size(), 0);
		}
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
