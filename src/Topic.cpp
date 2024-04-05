/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Topic.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvachera <mvachera@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/03 20:14:54 by mvachera          #+#    #+#             */
/*   Updated: 2024/04/04 20:37:49 by mvachera         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include <iostream>
#include <fcntl.h>

void	Server::topic_pars(Client *client, std::string buffer)
{
	size_t cmdEnd = buffer.find_first_of(" \t", 0);
	if (cmdEnd == std::string::npos)
		throw std::string("TOPIC: /TOPIC #name_of_the_channel new_topic <-(optional)");
	size_t channelStart = buffer.find_first_not_of(" \t#\r\n", cmdEnd);
	if (channelStart == std::string::npos)
		throw std::string("TOPIC: /TOPIC #name_of_the_channel new_topic <-(optional)");
	size_t channelEnd = buffer.find_first_of(" \t", channelStart);
	if (channelEnd == std::string::npos)
		throw std::string("TOPIC: /TOPIC #name_of_the_channel new_topic <-(optional)");
	std::string channel = buffer.substr(channelStart, channelEnd - channelStart);
	std::string newTopic;
	size_t topicStart = buffer.find_first_not_of(" \t\r\n", channelEnd);
	if (topicStart != std::string::npos)
	{
		size_t topicEnd = buffer.size();
		while (topicEnd > 0 && (buffer[topicEnd] == ' ' ||  buffer[topicEnd] == '\t'
			|| buffer[topicEnd] == '\r' || buffer[topicEnd] == '\n'))
			topicEnd--;
		newTopic = buffer.substr(topicStart, topicEnd - topicStart);
	}
	topic_exec(client, channel, newTopic);
}

void	Server::topic_exec(Client *client, std::string channel, std::string newTopic)
{
	Channel	*tmp = NULL;

	if (_channel.find(channel) != _channel.end())
		tmp = _channel[channel];
	if (tmp == NULL)
		throw std::string("TOPIC: Channel does not exist !");

	std::map<std::string, Client *> admins = tmp->getAdmins();
	std::map<std::string, Client *> clients = tmp->getClients();
	std::map<std::string, Client *>::iterator it1 = admins.find(client->getNickname());
	std::map<std::string, Client *>::iterator it2 = clients.find(client->getNickname());

	if (it1 == admins.end() && it2 == clients.end())
		throw std::string("TOPIC: Client not in the channel !");
	if (newTopic.empty() && tmp->getTopic().empty())
		throw std::string("TOPIC: Channel does not have a topic yet !");
	if (newTopic.empty())
		send(client->getSocket(), tmp->getTopic().c_str(), tmp->getTopic().size(), 0);
	else
		tmp->setTopic(newTopic);
}
