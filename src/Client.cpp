#include "Client.hpp"

Client::Client(){}

Client::~Client() {}

Client::Client(int fd, sockaddr_in addr) :  _address(addr), _socket_fd(fd), _register(false) , _check(0){}

std::string Client::getPass()
{
    return (_pass);
}
std::string Client::getUsername()
{
    return(_username);
}
std::string Client::getNickname()
{
    return(_nickname);
}
std::string Client::getRealname()
{
    return (_realname);
}
Client *Client::getClient()
{
    return(this);
}
bool Client::getRegister()
{
    return(_register);
}

int Client::getSocket()
{
    return (_socket_fd);
}


void    Client::setPassword(std::string pass) {
    _pass = pass;
}

void Client::setNickname(std::string nick)
{
    _nickname = nick;
}

void Client::setUsername(std::string username)
{
        _username = username;
}

void Client::setRegister(bool value)
{
    _register = value;
}

// std::map<std::string, Channel *> Client::getUserChannel()
// {
//     return (_userchannel);
// }
// bool Client::alreadylog(std::string channelname)
// {
//     if(_userchannel.find(channelname) != _userchannel.end())
//         return (true);
//     return (false);
// }

// void Client::addChannel(std::string channelname, std::string password)
// {
//     if(_userchannel.find(channelname) == _userchannel.end())
//     {
//         _userchannel[channelname] = new Channel(channelname, password);
//     }
// }
// void Client::rmChannel(std::string channelname)
// {
//     std::map<std::string, Channel *>::iterator it = _userchannel.find(channelname);
//     if(it != _userchannel.end())
//     {
//         delete it->second;
//         _userchannel.erase(it);
//     }
// }

void Client::checkplus()
{
    _check++;
}

int Client::getCheck()
{
    return(_check);
}