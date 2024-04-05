/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Mode.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvachera <mvachera@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/03 20:19:28 by mvachera          #+#    #+#             */
/*   Updated: 2024/04/04 20:56:49 by mvachera         ###   ########.fr       */
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
	(void)client;
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
	size_t checkRest = buffer.find_first_not_of(" \t\r\n", modeEnd);
	if (checkRest != std::string::npos)
		throw std::string("Mode: /MODE #name_of_the_channel (+ or -)option");
	if (mode.size() != 2)
		throw std::string("MODE: option doesn't exist");
	// mode_exec(client, channel, mode);
}

// void	Server::mode_exec(Client *client, std::string channel, std::string mode)
// {
// 	Channel	*tmp = NULL;

// 	if (_channel.find(channel) != _channel.end())
// 		tmp = _channel[channel];
// 	if (tmp == NULL)
// 		throw std::string("MODE: Channel does not exist !");

// 	std::map<std::string, Client *> admins = tmp->getAdmins();
// 	std::map<std::string, Client *> clients = tmp->getClients();
// 	std::map<std::string, Client *>::iterator it1 = admins.find(client->getNickname());
// 	std::map<std::string, Client *>::iterator it2 = clients.find(client->getNickname());

// 	if (it1 == admins.end() && it2 == clients.end())
// 		throw std::string("INVITE: Client not in the channel !");
// 	if (it1 == admins.end())
// 		throw std::string("INVITE: Client not an admin !");
// 	mode_option(client, channel, mode, tmp);
// }

// int	Server::check_option(const std::string &mode)
// {
// 	if (mode[1] == 'i')
// 		return (0);
// 	if (mode[1] == 't')
// 		return (1);
// 	if (mode[1] == 'k')
// 		return (2);
// 	if (mode[1] == 'o')
// 		return (3);
// 	if (mode[1] == 'l')
// 		return (4);
// 	return (-1);
// }

// void	Server::mode_option(Client *client, std::string channel, std::string mode, Channel *canal)
// {
// 	(void)client;
// 	switch (check_option(mode)) {
// 		case 0:
// 			i_mode(channel, mode, canal);
// 			break;
// 		case 1:
// 			t_mode(channel, mode, canal);
// 			break;
// 		case 2:
// 			k_mode(channel, mode, canal);
// 			break;
// 		case 3:
// 			o_mode(channel, mode, canal);
// 			break;
// 		case 4:
// 			l_mode(channel, mode, canal);
// 			break;
// 		default :
// 			throw std::string("MODE: option doesn't exist");
// 	}
// }

// void	Server::i_mode(std::string channel, std::string mode, Channel *canal)
// {
// 	if (mode[0] == '+')
// 	{
		
// 	}
// 	else
// 	{}
// }
