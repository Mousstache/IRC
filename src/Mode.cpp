/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Mode.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvachera <mvachera@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/03 20:19:28 by mvachera          #+#    #+#             */
/*   Updated: 2024/04/07 19:30:55 by mvachera         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include <iostream>
#include <fcntl.h>

void	Server::mode_pars(Client *client, std::string buffer)
{
	size_t cmdEnd = buffer.find_first_of(" \t", 0);
	if (cmdEnd == std::string::npos)
		throw std::string("Mode: /MODE #name_of_the_channel (+ or -)option");
	size_t channelStart = buffer.find_first_not_of(" \t#\r\n", cmdEnd);
	if (channelStart == std::string::npos)
		throw std::string("Mode: /MODE #name_of_the_channel (+ or -)option");
	size_t channelEnd = buffer.find_first_of(" \t", channelStart);
	if (channelEnd == std::string::npos)
		throw std::string("Mode: /MODE #name_of_the_channel (+ or -)option");
	std::string channel = buffer.substr(channelStart, channelEnd - channelStart);
	size_t modeStart = buffer.find_first_of("+-", channelEnd);
	if (modeStart == std::string::npos)
		throw std::string("Mode: /MODE #name_of_the_channel (+ or -)option");
	size_t modeEnd = buffer.find_first_of(" \t\r\n", modeStart);
	if (modeEnd == std::string::npos)
		throw std::string("Mode: /MODE #name_of_the_channel (+ or -)option");
	std::string mode = buffer.substr(modeStart, modeEnd - modeStart);
	std::string	name;
	size_t nameStart = buffer.find_first_not_of(" \t\r\n", modeEnd);
	if (nameStart != std::string::npos)
	{
		size_t nameEnd = buffer.find_first_of(" \t\r\n", modeEnd);
		if (nameEnd == std::string::npos)
			throw std::string("Mode: /MODE #name_of_the_channel (+ or -)option");
		name = buffer.substr(nameStart, nameEnd - nameStart);
		size_t checkRest = buffer.find_first_not_of(" \t\r\n", modeEnd);
		if (checkRest != std::string::npos)
			throw std::string("Mode: /MODE #name_of_the_channel (+ or -)option");
	}
	if (mode.size() != 2)
		throw std::string("MODE: option doesn't exist");
	if (!name.empty() && (mode[1] != 'o' || mode != "+l"))
		throw std::string("MODE: this option don't need a fourth argument");
	mode_exec(client, channel, mode, name);
}

void	Server::mode_exec(Client *client, std::string channel, std::string mode, std::string name)
{
	Channel	*tmp = NULL;

	if (_channel.find(channel) != _channel.end())
		tmp = _channel[channel];
	if (tmp == NULL)
		throw std::string("MODE: Channel does not exist !");

	std::map<std::string, Client *> admins = tmp->getAdmins();
	std::map<std::string, Client *> clients = tmp->getClients();
	std::map<std::string, Client *>::iterator it1 = admins.find(client->getNickname());
	std::map<std::string, Client *>::iterator it2 = clients.find(client->getNickname());

	if (it1 == admins.end() && it2 == clients.end())
		throw std::string("INVITE: Client not in the channel !");
	if (it1 == admins.end())
		throw std::string("INVITE: Client not an admin !");
	mode_option(client, mode, name, tmp);
}

int	Server::check_option(const std::string &mode)
{
	if (mode[1] == 'i')
		return (0);
	if (mode[1] == 't')
		return (1);
	if (mode[1] == 'k')
		return (2);
	if (mode[1] == 'o')
		return (3);
	if (mode[1] == 'l')
		return (4);
	return (-1);
}

void	Server::mode_option(Client *client, std::string mode, std::string name, Channel *canal)
{
	(void)client;
	switch (check_option(mode)) {
		case 0:
			i_mode(mode, canal);
			break;
		case 1:
			t_mode(mode, canal);
			break;
		case 2:
			k_mode(mode, canal);
			break;
		case 3:
			o_mode(mode, canal, name);
			break;
		case 4:
			l_mode(mode, canal, name);
			break;
		default :
			throw std::string("MODE: this option doesn't exist");
	}
}

void	Server::i_mode(std::string mode, Channel *canal)
{
	if (mode[0] == '+')
		canal->setInvitation(true);
	else
		canal->setInvitation(false);
}

void	Server::t_mode(std::string mode, Channel *canal)
{
	if (mode[0] == '+')
		canal->setAdmintopic(true);
	else
		canal->setAdmintopic(false);
}

void	Server::k_mode(std::string mode, Channel *canal)
{
	if (mode[0] == '+')
		canal->setPass(true);
	else
		canal->setPass(false);
}

void	Server::o_mode(std::string mode, Channel *canal, std::string name)
{
	Client	*user;
	std::map<std::string, Client *> admins = canal->getAdmins();
	std::map<std::string, Client *> clients = canal->getClients();
	std::map<std::string, Client *>::iterator it1 = admins.find(name);
	std::map<std::string, Client *>::iterator it2 = clients.find(name);
	user = it2->second;
	if (mode[0] == '+')
	{
		if (it1 == admins.end())
			admins[user->getNickname()] = user;
	}
	else
	{
		if (it1 != admins.end())
			admins.erase(admins.find(name));
	}
}

void	Server::l_mode(std::string mode, Channel *canal, std::string name)
{
	if (mode[0] == '+')
	{
		if (name.empty())
			throw std::string("MODE: you need to specify the limit of user for this channel !");
		int limitusers = atoi(name.c_str());
		if (limitusers == 0)
			throw std::string("MODE : wrong limit of users number !");
		canal->setLimituser(true, limitusers);
	}
	else
		canal->setLimituser(false, 0);
}
