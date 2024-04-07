/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Privmsg.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: motroian <motroian@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/03 20:14:22 by mvachera          #+#    #+#             */
/*   Updated: 2024/04/07 19:48:58 by motroian         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include <iostream>
#include <fcntl.h>

void	Server::privmsg_pars(Client *client, std::string buffer)
{
	size_t cmdEnd = buffer.find_first_of(" \t", 0);
	if (cmdEnd == std::string::npos)
		throw std::string("PRIVMSG: /msg user message");
	size_t userStart = buffer.find_first_not_of(" \t\r\n", cmdEnd);
	if (userStart == std::string::npos)
		throw std::string("PRIVMSG: /msg user message");
	size_t userEnd = buffer.find_first_of(" \t", userStart);
	if (userEnd == std::string::npos)
		throw std::string("PRIVMSG: /msg user message");
	std::string user = buffer.substr(userStart, userEnd - userStart);
	size_t msgStart = buffer.find_first_not_of(" \t\r\n", userEnd);
	if (msgStart == std::string::npos)
		throw std::string("PRIVMSG: /msg user message");
	size_t msgEnd = buffer.size();
	while (msgEnd > 0 && (buffer[msgEnd] == ' ' ||  buffer[msgEnd] == '\t'
		|| buffer[msgEnd] == '\r' || buffer[msgEnd] == '\n'))
		msgEnd--;
	std::string	msg = buffer.substr(msgStart, msgEnd - msgStart);
	privmsg_exec(client, user, msg);
}

// void	Server::privmsg_exec(Client *client, std::string user, std::string msg)
// {
// 	(void)client;
// 	Client	*tmp = NULL;
// 	std::map<int, Client *>::iterator it = _client.begin();

// 	for (it = _client.begin(); it != _client.end(); it++)
// 	{
// 		tmp = it->second;
// 		if (tmp->getNickname() == user)
// 			break ;
// 	}
// 	if (it == _client.end())
// 		throw std::string("PRIVMSG: user does not exist !");
// 	send(tmp->getSocket(), msg.c_str(), msg.size(), 0);
// }

void	Server::privmsg_exec(Client *client, std::string user, std::string msg)
{
	bool to_channel = false;
	bool send_file = false;
	int sock_test = client->getSocket();
	std::string desti;
	if (!client || user.empty() || msg.empty())
		return ;
	if (user[0] == '#')
	{
		desti = user.substr(1, user.size()-1);
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
		std::cout << "taille du chan privexec : " << _channel.size() << std::endl;
		if (_channel.find(desti) != _channel.end())
		{
			// std::cout << "name chan: " << _channel[desti]->getName() << "dest: " << user << std::endl;
    		if (_channel[desti]->getName() == desti)
			{
				for (std::map<int, Client*>::iterator it = _client.begin(); it != _client.end(); ++it) {
					Client* client = it->second;
					int clientSocket = client->getSocket();
					if (clientSocket != sock_test)
					{
						std::string to_send = RPL_PRIVMSG_CHANNEL(client->getNickname(), user, msg.c_str());
						std::cout << "le message : " << to_send << std::endl; 
						send(clientSocket, to_send.c_str(), to_send.size(), 0);
					}
				}
			}
			// _channel[desti]->chanmsg(msg);
		}
	}
	else
	{
		std::string to_send;
		for (std::map<int, Client *>::iterator it = _client.begin(); it != _client.end(); it++)
    	{
    	    if (it->second->getNickname() == user)
    	    {
				to_send = RPL_PRIVMSG_CLIENT(client->getNickname(), client->getUsername(), client->getRealname(), msg.c_str());
				send(client->getSocket(), to_send.c_str(), to_send.size(), 0); 	
    	    }
    	}
	}
}
