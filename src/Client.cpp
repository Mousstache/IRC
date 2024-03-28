#include "Client.hpp"

Client::Client(int fd, sockaddr_in addr) : socket_fd(fd), address(addr), _check(0), _register(false){}

Client::~Client(){}


bool const & Client::getRegister()
{
    return (_register);
}

std::string const & Client::getNickname()
{
    return (_nickname);
}
std::string const & Client::getPassword()
{
    return (_password);
}

std::string const & Client::getUsername()
{
        return (_username);
}

void Client::setRegister(bool reg)
{
    this->_register = reg;
}

void Client::setNickname(std::string str)
{
    this->_nickname = str;
}

void Client::setUsername(std::string str)
{
    this->_username = str;
}
void Client::setPassword(std::string str)
{
    this->_password = str;
}