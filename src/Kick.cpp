/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Kick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: motroian <motroian@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/03 20:20:50 by mvachera          #+#    #+#             */
/*   Updated: 2024/04/05 17:28:59 by motroian         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include <iostream>
#include <fcntl.h>

void Server::kick_pars(Client *client, std::string buffer)
{
	size_t cmdEnd = buffer.find_first_of(" \t", 0);
	if (cmdEnd == std::string::npos)
		throw std::string("KICK: /KICK #name_of_the_channel user reason <-(optional)");
	size_t channelStart = buffer.find_first_not_of(" \t#\r\n", cmdEnd);
	if (channelStart == std::string::npos)
		throw std::string("KICK: /KICK #name_of_the_channel user reason <-(optional)");
	size_t channelEnd = buffer.find_first_of(" \t", channelStart);
	if (channelEnd == std::string::npos)
		throw std::string("KICK: /KICK #name_of_the_channel user reason <-(optional)");
	std::string channel = buffer.substr(channelStart, channelEnd - channelStart);
	size_t userStart = buffer.find_first_not_of(" \t\r\n", channelEnd);
	if (userStart == std::string::npos)
		throw std::string("KICK: /KICK #name_of_the_channel user reason <-(optional)");
	size_t userEnd = buffer.find_first_of(" \t", userStart);
	if (userEnd == std::string::npos)
		throw std::string("KICK: /KICK #name_of_the_channel user reason <-(optional)");
	std::string user = buffer.substr(userStart, userEnd - userStart);
	std::string reason;
	size_t reasonStart = buffer.find_first_not_of(" \t\r\n", userEnd);
	if (reasonStart != std::string::npos)
	{
		size_t reasonEnd = buffer.size();
		while (reasonEnd > 0 && (buffer[reasonEnd] == ' ' ||  buffer[reasonEnd] == '\t'
			|| buffer[reasonEnd] == '\r' || buffer[reasonEnd] == '\n'))
			reasonEnd--;
		reason = buffer.substr(reasonStart, reasonEnd - reasonStart);
	}
	kick_exec(client, channel, user, reason);
}

void Server::kick_exec(Client *client, std::string channel, std::string name, std::string reason)
{
	Channel	*tmp = NULL;
	if (_channel.find(channel) != _channel.end())
		tmp = _channel[channel];
	std::map<std::string, Client *> admins = tmp->getAdmins();
	std::map<std::string, Client *> clients = tmp->getClients();
	std::map<std::string, Client *>::iterator it1 = admins.find(client->getNickname());
	std::map<std::string, Client *>::iterator it2 = clients.find(client->getNickname());
	std::map<std::string, Client *>::iterator it3 = admins.find(name);
	std::map<std::string, Client *>::iterator it4 = clients.find(name);

	if (tmp == NULL)
		throw std::string("KICK: Channel does not exist !");
	if (it1 == tmp->getAdmins().end() && it2 == tmp->getClients().end())
		throw std::string("KICK: Client is not in the channel !");
	if (it1 == tmp->getAdmins().end())
		throw std::string("KICK: Client cannot kick user if not admin !");
	if (it3 == tmp->getAdmins().end() && it4 == tmp->getClients().end())
		throw std::string("KICK: User not in the channel !");
	if (it3 == tmp->getAdmins().end())
		throw std::string("KICK: Client cannot kick an admin user !");
	if (tmp->getClients().find(name) != tmp->getClients().end())
        tmp->getClients().erase(tmp->getClients().find(name));
	if (!reason.empty())
	{
		std::string	msg = client->getNickname() + " has been kick because " + reason;
		send(client->_socket_fd, msg.c_str(), msg.size(), 0);
	}
	else
	{
		std::string	msg = client->getNickname() + " has been kick";
		send(client->_socket_fd, msg.c_str(), msg.size(), 0);
	}
}
