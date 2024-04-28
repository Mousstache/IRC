/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Topic.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: motroian <motroian@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/03 20:14:54 by mvachera          #+#    #+#             */
/*   Updated: 2024/04/27 19:07:55 by motroian         ###   ########.fr       */
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
	size_t topicStart = buffer.find_first_of(":", channelEnd);
	if (topicStart != std::string::npos)
	{
		topicStart++;
		size_t topicEnd = buffer.find_first_of("\r\n", topicStart);
		if (topicEnd == std::string::npos)
			throw std::string("TOPIC: /TOPIC #name_of_the_channel new_topic <-(optional)");
		newTopic = buffer.substr(topicStart, topicEnd - topicStart);
	}
	topic_exec(client, channel, newTopic);
}

void	Server::topic_exec(Client *client, std::string channel, std::string newTopic)
{
	Channel	*tmp = NULL;
	std::ostringstream oss;
    oss << std::time(0);
    std::string currentTime = oss.str();

	if (_channel.find(channel) != _channel.end())
		tmp = _channel[channel];
	if (tmp == NULL)
	{
		std::string err = ERR_NOSUCHCHANNEL(client->getNickname(), channel);
		send(client->getSocket(), err.c_str(), err.size(), 0);
		throw std::string("TOPIC: Channel does not exist !");
	}

	std::map<std::string, Client *> admins = tmp->getAdmins();
	std::map<std::string, Client *> clients = tmp->getClients();
	std::map<std::string, Client *>::iterator it1 = admins.find(client->getNickname());
	std::map<std::string, Client *>::iterator it2 = clients.find(client->getNickname());

	if (it1 == admins.end() && it2 == clients.end())
	{
		std::string err = ERR_NOTONCHANNEL(client->getNickname(), channel);
		send(client->getSocket(), err.c_str(), err.size(), 0);
		throw std::string("TOPIC: Client not in the channel !");
	}
	if (tmp->getAdmintopic() && it1 == admins.end() && !newTopic.empty())
	{
		std::string err = ERR_CHANOPRIVSNEED(client->getNickname(), channel);
		send(client->getSocket(), err.c_str(), err.size(), 0);
        throw std::string("TOPIC: Only the admins can change the topic on this channel !");
	}
	if (newTopic.size() == 0 && tmp->getTopic().empty())
	{
		std::string err = RPL_NOTOPIC(client->getNickname(), channel);
		send(client->getSocket(), err.c_str(), err.size(), 0);
		throw std::string("TOPIC: Channel does not have a topic yet !");
	}
	if (newTopic.size() == 0)
	{
		std::string err = RPL_TOPIC(client->getNickname(), channel, tmp->getTopic());
		std::string totime = RPL_TOPICWHOTIME(client->getUsername(), channel, client->getNickname(), currentTime);
		tmp->chanmsg(err, "");	
		tmp->chanmsg(totime, "");
	}
	else
	{
		std::cout << "NEW TOPIC" << std::endl;
		tmp->setTopic(newTopic);
		std::cout << currentTime << std::endl;
		std::string err = RPL_TOPIC(client->getNickname(), channel, tmp->getTopic());
		std::string totime = RPL_TOPICWHOTIME(client->getUsername(), channel, client->getNickname(), currentTime);
		tmp->chanmsg(err, "");	
		tmp->chanmsg(totime, "");
	}
}
