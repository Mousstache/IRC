/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Nick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: motroian <motroian@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/05 17:35:05 by mvachera          #+#    #+#             */
/*   Updated: 2024/04/27 19:19:40 by motroian         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include <iostream>
#include <fcntl.h>


void    Server::nick_pars(Client *client, std::string buffer)
{
    size_t cmdEnd = buffer.find_first_of(" \t", 0);
    if (cmdEnd == std::string::npos)
        throw std::string("NICK: /NICK user");
    size_t userStart = buffer.find_first_not_of(" \t\r\n", cmdEnd);
    if (userStart == std::string::npos)
        throw std::string("NICK: /NICK user");
    size_t userEnd = buffer.find_first_of(" \t\r\n", userStart);
    if (userEnd == std::string::npos)
        throw std::string("NICK: /NICK user");
    std::string user = buffer.substr(userStart, userEnd - userStart);
    size_t checkRest = buffer.find_first_not_of(" \t\r\n", userEnd);
    if (checkRest != std::string::npos)
        throw std::string("NICK: /NICK user");
    for (size_t i = 0; i < user.size(); i++)
    {
        char c = user[i];
        if (!std::isalnum(c))
            throw std::string("NICK: The new name contain an non alphanumeric caractere !");
    }
    for (std::map<int, Client *>::iterator it = _client.begin(); it != _client.end(); it++)
    {
        if (it->second->getNickname() == user)
            throw std::string("NICK: This name is already use by another client !");
    }
    nick_exec(client, user);
}

void    Server::nick_exec(Client *client, std::string user)
{
    std::vector<int> flags;
    for (std::map<std::string, Channel *>::iterator it = _channel.begin(); it != _channel.end(); it++)
    {
        int flag = 0;
        if (it->second->getInvitUser(client->getNickname()) != NULL)
        {
            it->second->rmInvit(client->getNickname());
            flag = 1;
        }
		if (it->second->getIencli(client->getNickname()) != NULL)
        {
            it->second->rmClients(client->getNickname());
            flag = 2;
        }
        if (it->second->getAdminUser(client->getNickname()) != NULL)
        {
            it->second->rmAdmins(client->getNickname());
            flag = 3;
        }
        flags.push_back(flag);
    }
	std::string nick = NICK_RPL(client->getNickname(), client->getUsername(), user);
	send(client->getSocket(), nick.c_str(), nick.size(), 0);
    client->setNickname(user);
    int i = 0;
    for (std::map<std::string, Channel *>::iterator it = _channel.begin(); it != _channel.end(); it++)
    {
        if (flags[i] == 1)
            it->second->addInvits(client);
        else if (flags[i] == 2)
            it->second->addClients(client);
        else if (flags[i] == 3)
        {
            it->second->addClients(client);
            it->second->addAdmins(client);
        }
        i++;	
    }
    flags.clear();
}
