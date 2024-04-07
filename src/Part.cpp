/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Part.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvachera <mvachera@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/05 17:35:05 by mvachera          #+#    #+#             */
/*   Updated: 2024/04/05 17:46:51 by mvachera         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include <iostream>
#include <fcntl.h>

void	Server::part_pars(Client *client, std::string buffer)
{
	size_t cmdEnd = buffer.find_first_of(" \t", 0);
	if (cmdEnd == std::string::npos)
		throw std::string("PART: /PART #nom_du_canal");
	size_t channelStart = buffer.find_first_not_of(" \t#\r\n", cmdEnd);
	if (channelStart == std::string::npos)
		throw std::string("PART: /PART #nom_du_canal");
	size_t channelEnd = buffer.find_first_of(" \t", channelStart);
	if (channelEnd == std::string::npos)
		throw std::string("PART: /PART #nom_du_canal");
	std::string channel = buffer.substr(channelStart, channelEnd - channelStart);
	size_t checkRest = buffer.find_first_not_of(" \t\r\n", channelEnd);
	if (checkRest != std::string::npos)
		throw std::string("PART: /PART #nom_du_canal");
	part_exec(client, channel);
}

void	Server::part_exec(Client *client, std::string channel)
{
	Channel	*tmp = NULL;

	if (_channel.find(channel) != _channel.end())
		tmp = _channel[channel];
	if (tmp == NULL)
		throw std::string("PART: Channel does not exist !");

	std::map<std::string, Client *> admins = tmp->getAdmins();
	std::map<std::string, Client *> clients = tmp->getClients();
	std::map<std::string, Client *>::iterator it1 = admins.find(client->getNickname());
	std::map<std::string, Client *>::iterator it2 = clients.find(client->getNickname());

	if (it1 == admins.end() && it2 == clients.end())
		throw std::string("TOPIC: Client not in the channel !");
	if (it1 != admins.end() && admins.size() > 1)
	{
		// donner les droit d'admins a un autre user
	}
	if (tmp->getClients().find(client->getNickname()) != tmp->getClients().end())
		tmp->getClients().erase(tmp->getClients().find(client->getNickname()));
}
