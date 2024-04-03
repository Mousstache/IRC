#include "Server.hpp"
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include <iostream>
#include <fcntl.h> 

void Server::exitWithError(std::string errorMessage)
{
	perror(errorMessage.c_str());
}


std::string Server::getPassword()
{
	return (_password);
}
void Server::printserv() {}
Client *Server::getClient(std::string name)
{
	std::map<int, Client *>::iterator it = _client.begin();
	while(it != _client.end())
	{
		if (it->second->getUsername() == name)
		{
			return (it->second);
		}
		it++;
	}
	return (NULL);
}
Channel *Server::getChannel(std::string channelname)
{
	if (_channel.find(channelname) != _channel.end())
	{
		return (_channel[channelname]);
	}
	return (NULL);
}


void Server::log(const std::string &message)
{
	std::cout << message << std::endl;
}

Server::Server(std::string port, std::string password)
{
	_socket = -1;
	_bin = -1;
	_lis = -1;
	_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (_socket < 0)
	{
		exitWithError("Error establishing the server socket");
	}
	int flags = fcntl(_socket, F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl");
        return;
    }
    if (fcntl(_socket, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("fcntl");
        return;
    }
	_port = atoi(port.c_str());
	_password = password;
	if (_port < 1024 || _port > 65535)
	{
		std::cout << "bad value of port" << std::endl;
		return ;
	}
	if (_socket < 0)
	{
		std::cerr << "sock error" << std::endl;
		return ;
	}
	memset((char *)&_sin, 0, sizeof(_sin)); // Use memset instead of bzero
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(_port);
	_sin.sin_addr.s_addr = INADDR_ANY;
	_bin = bind(_socket, (struct sockaddr *)&_sin, sizeof(_sin));
	if (_bin < 0)
	{
		return;
		// exitWithError("Error binding socket to local address");
	}
	_lis = listen(_socket, 5);
	if (_lis)
	{
		std::cerr << "listen error" << std::endl;
		return ;
	}
}

Server::~Server()
{
//    std::cout << "Wesh" << std::endl;
	// delete _client["new"];
    for (std::map<int , Client*>::iterator it = _client.begin(); it != _client.end(); it++)
    {
        delete it->second;
    }
    _client.clear();
    for (std::map<std::string , Channel*>::iterator it = _channel.begin(); it != _channel.end(); it++)
    {
        delete it->second;
    }
	_channel.clear();
}



void Server::serving() {
    int max_sd, new_socket;
    fd_set rfds, tmp_rfds;
    struct sockaddr_in client;
    socklen_t addr_len = sizeof(client);

    FD_ZERO(&rfds);
    FD_SET(_socket, &rfds);
    max_sd = _socket;

    std::cout << "Server is waiting for connections on port " << _port << std::endl;


    while (server_off == false)
    {
    	tmp_rfds = rfds;
		if (select(max_sd + 1, &tmp_rfds, NULL, NULL, NULL) == -1) {
			return;
		}
        if (FD_ISSET(_socket, &tmp_rfds)) {
            if ((new_socket = accept(_socket, (struct sockaddr *)&client, &addr_len)) == -1) {
                std::cerr << "accept error" << std::endl;
                return;
            } else {
                std::cout << "New connection from " << inet_ntoa(client.sin_addr) << " on socket " << new_socket << std::endl;
				_client[new_socket] = new Client(new_socket, client);
                FD_SET(new_socket, &rfds);
                if (new_socket > max_sd)
                    max_sd = new_socket;
            }
        }
        for (std::map<int , Client*>::iterator it = _client.begin(); it != _client.end();)
        {
            int client_socket = it->first;
            if (FD_ISSET(client_socket, &tmp_rfds)) {
                char buffer[1024];
                int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
                if (bytes_received <= 0)
                {
                    std::cout << "Client on socket " << client_socket << " " << it->second->getUsername() << " disconnected." << std::endl;
                    FD_CLR(client_socket, &rfds);
					delete it->second;
					_client.erase(it);
					it = _client.begin();
					close(client_socket);
                }
                else
                {
                    buffer[bytes_received] = '\0';
                    std::cout << std::endl;
                    parse(it->second, buffer);
                    std::cout << "Received from client " << client_socket << ": [" << buffer << "]" << std::endl;
                    ++it;
                }
            }
			else
			{
                ++it;
            }
        }
    }
    close(_socket);
}


int	cmdparser(const std::string &str)
{
	if (str.compare(0, 4, "PASS") == 0)
		return (4);
	if (str.compare(0, 4, "NICK") == 0)
		return (4);
	if (str.compare(0, 4, "USER") == 0)
		return (4);
	return (0);
}

void Server::exec_cmd(std::string const &command, std::string const &value,
	Client *client)
{
	if (command == "PASS")
	{
		if (value != this->getPassword())
		{
			std::cout << "BAD PASSWORD" << std::endl;
		}
		client->setPassword(value);
		client->checkplus();
	}
	else if (command == "NICK")
	{
		client->setNickname(value);
		client->checkplus();
	}
	else if (command == "USER")
	{
		client->setUsername(value);
		client->checkplus();
	}
}

void Server::set_id(std::string str, Client *client)
{
	size_t	i;
	size_t	cmdStart;
	size_t	cmdEnd;
	size_t	valueStart;
	size_t	valueEnd;

	i = 0;
	while (i < str.size())
	{
		cmdStart = str.find_first_of("PASS, NICK, USER", i);
		if (cmdStart == std::string::npos)
			break ;
		cmdEnd = str.find_first_of(" \r\n", cmdStart);
		if (cmdEnd == std::string::npos)
			break ;
		std::string command = str.substr(cmdStart, cmdEnd - cmdStart);
		std::cout << "Command: " << command << std::endl;
		valueStart = str.find_first_not_of(" \r\n", cmdEnd);
		if (valueStart == std::string::npos)
			break ;
		valueEnd = str.find_first_of(" \r\n", valueStart);
		if (valueEnd == std::string::npos)
			break ;
		std::string value = str.substr(valueStart, valueEnd - valueStart);
		std::cout << "Value: " << value << std::endl;
		if (cmdparser(command) == 4)
		{
			exec_cmd(command, value, client);
		}
		i = valueEnd + 2;
	}
}
bool Server::cmd_pars(Client *client, std::string buffer)
{
	size_t	i;
	size_t	cmdStart;
	size_t	cmdEnd;

	(void)client;
	i = 0;
	cmdStart = buffer.find_first_of("JOIN, KICK, INVITE, TOPIC, MODE, PING, PART", i);
	std::string passvalue;
	if (cmdStart == std::string::npos)
		return (false);
	cmdEnd = buffer.find_first_of(" ", cmdStart);
	if (cmdEnd == std::string::npos)
		return (false);
	std::string command = buffer.substr(cmdStart, cmdEnd - cmdStart);
	std::cout << "Command: " << command << std::endl;
	if (command == "JOIN")
	{
		if (join_pars(client, buffer))
		{
			return (true);
		}
		return (false);
	}
	else if (command == "KICK")
	{
		if (kick_pars(client, buffer))
		{
			std::cout << "[" << command << "]" << std::endl;
			return (true);
		}
		return (false);
	}
	else if (command == "INVITE")
	{
		invite_pars(client, buffer);
		std::cout << "[" << command << "]" << std::endl;
	}
	else if (command == "TOPIC")
	{
		std::cout << "[" << command << "]" << std::endl;
	}
	else if (command == "MODE")
	{
		std::cout << "[" << command << "]" << std::endl;
	}
	else if (command == "PRIVMSG")
	{
		std::cout << "[" << command << "]" << std::endl;
		privmsg_pars(client,buffer);

	}
	else if (command == "PING")
	{
		ping_pars(client, buffer);
	}
	else if (command == "PART")
	{
		std::cout << "ICI" << std::endl;
		part_parse(client, buffer);
	}
	return (false);
}

bool Server::part_parse(Client *client, std::string buffer)
{
	size_t	i;
	size_t	cmdStart;
	size_t	cmdEnd;
	size_t	valueStart;
	size_t	valueEnd;
	size_t	pass;
	size_t	passend;

	i = 0;
	cmdStart = buffer.find_first_of("PART", i);
	cmdEnd = buffer.find_first_of(" ", cmdStart);
	if (cmdStart == std::string::npos || cmdEnd == std::string::npos)
		return (false);
	std::string command = buffer.substr(cmdStart, cmdEnd - cmdStart);
	std::cout << "Command: " << command << std::endl;
	valueStart = buffer.find_first_not_of(" ", cmdEnd);
	valueEnd = buffer.find_first_of(" ", valueStart);
	if (valueEnd == std::string::npos || valueStart == std::string::npos)
		return (false);
	std::string dest = buffer.substr(valueStart, valueEnd - valueStart);
	std::cout << "Dest: " << dest << std::endl;
	if (command != "PART")
		return (false);
	pass = buffer.find_first_not_of(" :", valueEnd);
	passend = buffer.find_first_of(" \r\n", pass);
	if (passend == std::string::npos || pass == std::string::npos)
	{
		std::cout << "PLOUF" << std::endl;
		return (false);
	}
	std::string msg = buffer.substr(pass, passend - valueStart);
	std::cout << "msg == " << msg << std::endl;
	return (part_exec(client, dest, msg));

}

bool Server::part_exec(Client *client,std::string channel,std::string msg)
{
	(void)msg;
	if (_channel[channel]->clientExist(client->getNickname()) == false)
		return (false);
	std::string message = ":" + client->getUsername() + "!~" + client->getUsername() + "@localhost PART :" + channel + "\r\n";
	send(client->_socket_fd, message.c_str(), message.size(), 0);
	return (true);
}

bool Server::ping_pars(Client *client, std::string buffer)
{
	(void)buffer;
	std::string pong = "PONG irc.example.com \r\n";
	send(client->_socket_fd, pong.c_str(), pong.size(), 0);
	std::cout << pong << std::endl;
	return (true);
}
bool Server::privmsg_pars(Client *client, std::string buffer)
{
	size_t	i;
	size_t	cmdStart;
	size_t	cmdEnd;
	size_t	valueStart;
	size_t	valueEnd;
	size_t	pass;
	size_t	passend;

	i = 0;
	cmdStart = buffer.find_first_of("PRIVMSG", i);
	cmdEnd = buffer.find_first_of(" ", cmdStart);
	if (cmdStart == std::string::npos || cmdEnd == std::string::npos)
		return (false);
	std::string command = buffer.substr(cmdStart, cmdEnd - cmdStart);
	std::cout << "Command: " << command << std::endl;
	valueStart = buffer.find_first_not_of(" ", cmdEnd);
	valueEnd = buffer.find_first_of(" ", valueStart);
	if (valueEnd == std::string::npos || valueStart == std::string::npos)
		return (false);
	std::string dest = buffer.substr(valueStart, valueEnd - valueStart);
	std::cout << "Dest: " << dest << std::endl;
	if (command != "PRIVMSG")
		return (false);
	pass = buffer.find_first_not_of(" :", valueEnd);
	passend = buffer.find_first_of(" \r\n", pass);
	if (passend == std::string::npos || pass == std::string::npos)
		return (false);
	std::string msg = buffer.substr(pass, passend - valueStart);
	return (privmsg_exec(client, dest, msg));
}

bool Server::privmsg_exec(Client *client, std::string dest, std::string msg)
{
	bool to_channel = false;
	bool send_file = false;
	std::string desti;
	if (!client || dest.empty() || msg.empty())
		return (false);
	if (dest[0] == '#')
	{
		desti = dest.substr(1, dest.size()-1);
		to_channel = true;
		// if (_channel[desti]->clientExist(client->getNickname()) == false)
		// {
		// 	return (false);
		// }
	}
	if (size_t find = msg.find_first_of("DCC") != std::string::npos)
		send_file = true;
	if (send_file)
	{
		//envoie un fichier
	}
	else if (to_channel)
	{
		std::cout << "je rentre dans chan : " << desti << std::endl; 
		std::cout << _channel.size() << std::endl;
		if (_channel.find(desti) != _channel.end())
		{
			std::cout << "name chan: " << _channel[desti]->getName() << "dest: " << dest << std::endl;
    		if (_channel[desti]->getName() == desti)
    		{
				std::cout << "bonjouw\n";
				std::string to_send = "salam";	
				// to_send = RPL_PRIVMSG_CLIENT(client->getNickname(), client->getUsername(), client->getRealname(), str.c_str());
				_channel[desti]->print();
				_channel[desti]->chanmsg(to_send);
    			   //  return (false);
    		}
		}
	}
	else
	{
		std::cout << "auwvoiw\n";
		std::string to_send;
		std::string str = "salam";
		for (std::map<int, Client *>::iterator it = _client.begin(); it != _client.end(); it++)
    	   {
    	       if (it->second->getNickname() == dest)
    	       {
					to_send = RPL_PRIVMSG_CLIENT(client->getNickname(), client->getUsername(), client->getRealname(), str.c_str());
					send(client->getSocket(), to_send.c_str(), to_send.size(), 0); 	
    	           return (false);
    	       }
    	   }
	}
	return (true);
}


bool Server::join_pars(Client *client, std::string buffer)
{
	size_t	i;
	size_t	cmdStart;
	size_t	cmdEnd;
	size_t	valueStart;
	size_t	valueEnd;
	size_t	pass;
	size_t	passend;

	i = 0;
	cmdStart = buffer.find_first_of("JOIN", i);
	std::string passvalue;
	if (cmdStart == std::string::npos)
		return (false);
	cmdEnd = buffer.find_first_of(" ", cmdStart);
	if (cmdEnd == std::string::npos)
		return (false);
	std::string command = buffer.substr(cmdStart, cmdEnd - cmdStart);
	std::cout << "Command: " << command << std::endl;
	valueStart = buffer.find_first_not_of(" #", cmdEnd);
	if (valueStart == std::string::npos)
		return (false);
	valueEnd = buffer.find_first_of(" \r\n", valueStart);
	if (valueEnd == std::string::npos)
		return (false);
	std::string value = buffer.substr(valueStart, valueEnd - valueStart);
	std::cout << "Value: " << value << std::endl;
	if (command != "JOIN")
		return (false);
	pass = buffer.find_first_not_of(" ", valueEnd);
	passend = buffer.find_first_of(" \r\n", pass);
	if (passend != std::string::npos && pass != std::string::npos)
	{
		passvalue = buffer.substr(pass, passend - valueStart);
		std::cout << "pass------------------>: " << passvalue << std::endl;
	}
	else
	{
		std::string badchan = ":irc.example.com 432 Vous n'Ãªtes pas sur ce canal.\r\n";
		send(client->_socket_fd, badchan.c_str(), badchan.size(), 0);
		std::cout << "Erreur" << std::endl;
		return (false);
	}
	std::cout << "value " << value << " password  " << passvalue << std::endl;
	return (joinChannel(client, value, passvalue));
}

bool	Server::invite_pars(Client *client, std::string buffer)
{
	std::string	str[3];
	std::stringstream	lf(buffer);
	char	c;

	std::getline(lf, str[0], ' ');
	if (str[0] != "INVITE")
		return (false);
	while (lf.get(c))
	{
		if (!std::isspace(c))
		{
			lf.putback(c);
			break ;
		}
	}
	std::getline(lf, str[1], ' ');
	if (str[1].empty())
		return (false);
	while (lf.get(c))
	{
		if (c == '#')
			break ;
		if (c != '#' && !std::isspace(c))
			return (false);
	}
	std::getline(lf, str[2], ' ');
	if (str[2].empty())
		return (false);
	while (lf.get(c))
	{
		if (!std::isspace(c) && c != '\0')
			return (false);
	}
	invite_exec(client, str[1], str[2]);
	return (true);
}

void	Server::invite_exec(Client *client, std::string user, std::string channel)
{
	Channel	*tmp = NULL;
	Client	*tmp2 = NULL;

	std::cout << "channel : " << channel <<std::endl;
	std::cout << "channel : " << _channel.size() <<std::endl;
	if (_channel.find(channel) != _channel.end())
		tmp = _channel[channel];
	std::map<int, Client *>::iterator it = _client.begin();
	while(it != _client.end())
	{
		if (it->second->getUsername() == user)
		{
			tmp2 = it->second;
		}
		it++;
	}

	if (tmp == NULL)
	{
		std::cout << "INVITE: Channel does not exist !" << std::endl;
		return ;
	}
	if (tmp2 == NULL)
	{
		std::cout << "INVITE: User does not exist !" << std::endl;
		return ;
	}

	std::map<std::string, Client *> admins = tmp->getAdmins();
	std::map<std::string, Client *> clients = tmp->getClients();
	std::map<std::string, Client *>::iterator it1 = admins.find(client->getUsername());
	std::map<std::string, Client *>::iterator it2 = clients.find(client->getUsername());
	std::map<std::string, Client *>::iterator it3 = admins.find(user);
	std::map<std::string, Client *>::iterator it4 = clients.find(user);

	if (it1 == admins.end() && it2 == clients.end())
	{
		std::cout << "INVITE: Client not in the channel !" << std::endl;
		return ;
	}
	if (it3 != admins.end() || it4 != clients.end())
	{
		std::cout << "INVITE: User already in the channel !" << std::endl;
		return ;
	}
	std::string msg = RPL_INVITING(client->getNickname(), user, channel);
    std::string msg2 = INVITE_CLIENT(client->getNickname(), client->getUsername(), "INVITE", user, channel);
    send(client->getSocket(), msg.c_str(), msg.size(), 0);
    send(it2->second->getSocket(), msg2.c_str(), msg2.size(), 0);
	// std::string invite = RPL_INVITING(it1, it2, it3);
	// send(clients, invite, tmp, 0);
	// send the invitation to the user
}

bool Server::kick_pars(Client *client, std::string buffer)
{
	std::string	str[4];
	std::stringstream	lf(buffer);
	char	c;

	std::getline(lf, str[0], ' ');
	if (str[0] != "KICK")
		return (false);
	while (lf.get(c))
	{
		if (c == '#')
			break ;
		if (c != '#' && !std::isspace(c))
			return (false);
	}
	std::getline(lf, str[1], ' ');
	if (str[1].empty())
		return (false);
	while (lf.get(c))
	{
		if (!std::isspace(c))
		{
			lf.putback(c);
			break ;
		}
	}
	std::getline(lf, str[2], ' ');
	if (str[2].empty())
		return (false);
	while (lf.get(c))
	{
		if (c != '\0' && !std::isspace(c))
		{
			std::getline(lf, str[3], '\0');
			break ;
		}
		if (c == '\0')
			break ;
	}
	kick_exec(client, str[1], str[2], str[3]);
	return (true);
}

void Server::kick_exec(Client *client, std::string channel, std::string name, std::string raison)
{
	Channel	*tmp = NULL;
	if (_channel.find(channel) != _channel.end())
		tmp = _channel[channel];
	std::map<std::string, Client *> admins = tmp->getAdmins();
	std::map<std::string, Client *> clients = tmp->getClients();
	std::map<std::string, Client *>::iterator it1 = admins.find(client->getUsername());
	std::map<std::string, Client *>::iterator it2 = clients.find(client->getUsername());
	std::map<std::string, Client *>::iterator it3 = admins.find(name);
	std::map<std::string, Client *>::iterator it4 = clients.find(name);

	if (tmp == NULL)
	{
		std::cout << "KICK: Channel does not exist !" << std::endl;
		return ;
	}
	if (it1 == tmp->getAdmins().end() && it2 == tmp->getClients().end())
	{
		std::cout << "KICK: Client is not in the channel !" << std::endl;
		return ;
	}
	if (it1 == tmp->getAdmins().end())
	{
		std::cout << "KICK: Client cannot kick user if not admin !" << std::endl;
		return ;
	}
	if (it3 == tmp->getAdmins().end() && it4 == tmp->getClients().end())
	{
		std::cout << "KICK: User not in the channel !" << std::endl;
		return ;
	}
	if (it4 == tmp->getAdmins().end())
	{
		std::cout << "KICK: Client cannot kick an admin user !" << std::endl;
		return ;
	}
	delete it4->second;
	tmp->getClients().erase(it4);
	std::cout << "KICK: User has been kick ";
	if (!raison.empty())
		std::cout << "because" << raison << " ";
	std::cout << "!" << std::endl;
}

void Server::parse(Client *client, std::string buffer)
{
	size_t	bufferSize;

	bufferSize = buffer.size();
	if (buffer[bufferSize - 1] == '\n' && buffer[bufferSize - 2] == '\r')
	{
		if (!client->getRegister())
		{
			set_id(buffer, client);
			std::cout << client->getUsername() << " check = " << client->getCheck() << std::endl;
			if (client->getCheck() == 3)
			{
				std::string welcome = ":irc.example.com 001 "
					+ client->getNickname()
					+ " :Welcome to the IRC Network BRO\r\n";
				send(client->_socket_fd, welcome.c_str(), welcome.size(), 0);
				client->setRegister(true);
				return ;
			}
		}
		else if (client->getRegister())
		{
			std::cout << "BUFFER = " << buffer << std::endl;
			if (cmd_pars(client, buffer))
			{
				std::cout << "BUFFER = " << buffer << std::endl;
			}
			else
			{
				// buffer.clear();
			}
			return ;
		}
	}
}



bool Server::joinChannel(Client *client, std::string channelname,std::string password)
{
	if (client)
	{
		if (_channel.find(channelname) == _channel.end())
		{
            std::cout << "created channel" << std::endl;
			_channel[channelname] = new Channel(client, channelname, password);
			_channel[channelname]->addAdmins(client);
			_channel[channelname]->addClients(client);
			std::string goodjoin = ":" + client->getNickname() + "!"
	            + client->getUsername() + "@localhost JOIN :#" + _channel[channelname]->getName() + "\r\n";
	            send(client->_socket_fd, goodjoin.c_str(), goodjoin.size(), 0);
		}
		else
		{
			if (_channel[channelname]->getNeed() && _channel[channelname]->getPassword() != password)
			{
                std::cout << "NOOOOOO" << std::endl;
                std::string error_passwd = ":ERR_BADCHANNELKEY" +_channel[channelname]->getName() + " :Password incorrect\r\n";
                send(client->_socket_fd, error_passwd.c_str(), error_passwd.size(), 0);
				return false;
			}
				std::string goodjoin = ":" + client->getNickname() + "!"
	            + client->getUsername() + "@localhost JOIN :#" + _channel[channelname]->getName() + "\r\n";
	            send(client->_socket_fd, goodjoin.c_str(), goodjoin.size(), 0);
            _channel[channelname]->addClients(client);
		}
	}
	_channel[channelname]->print();
	return true;
}