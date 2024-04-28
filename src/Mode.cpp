/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Mode.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: motroian <motroian@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/03 20:19:28 by mvachera          #+#    #+#             */
/*   Updated: 2024/04/28 20:10:09 by motroian         ###   ########.fr       */
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
	std::vector<std::string> arg;
	size_t nameStart = buffer.find_first_not_of(" \t\r\n", modeEnd);
	if (nameStart != std::string::npos)
	{
		while (1)
		{
			size_t nameEnd = buffer.find_first_of(" \t\r\n", nameStart);
			if (nameEnd == std::string::npos)
				throw std::string("Mode: /MODE #name_of_the_channel (+ or -)option");
			std::string name = buffer.substr(nameStart, nameEnd - nameStart);
			arg.push_back(name);
			nameStart = buffer.find_first_not_of(" \t\r\n", nameEnd);
			if (nameStart == std::string::npos)
				break ;
		}
	}
	mode_exec(client, channel, mode, arg);
}

void	Server::mode_exec(Client *client, std::string channel, std::string mode, std::vector<std::string> arg)
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
		throw std::string("MODE: Client not in the channel !");
	if (it1 == admins.end())
	{
		std::string err = ERR_CHANOPRIVSNEED(client->getNickname(), channel);
		send(client->getSocket(), err.c_str(), err.size(),0);
		throw std::string("MODE: Client not an admin !");
	}
	mode_option(client, mode, arg, tmp);
}

int	Server::check_option(char option)
{
	if (option == 'i')
		return (0);
	if (option == 't')
		return (1);
	if (option == 'k')
		return (2);
	if (option == 'o')
		return (3);
	if (option == 'l')
		return (4);
	if (option == '+')
		return (5);
	if (option == '-')
		return (6);
	return (-1);
}

void	Server::mode_option(Client *client, std::string mode, std::vector<std::string> arg, Channel *canal)
{
	int flag = 0;
	size_t	j = 0;

	for (size_t i = 0; i < mode.length(); i++)
	{
		switch (check_option(mode[i])) {
			case 0:
				i_mode(flag, canal, client);
				break;
			case 1:
				t_mode(flag, canal, client);
				break;
			case 2:
				if (flag == 0)
					k_mode2(canal, client);
				else if (j < arg.size())
				{
					k_mode(canal, arg[j], client);
					j++;
				}
				break;
			case 3:
				if (j < arg.size())
					o_mode(flag, canal, arg[j], client);
				j++;
				break;
			case 4:
				if (flag == 0)
					l_mode2(canal, client);
				else if (j < arg.size())
				{
					l_mode(canal, arg[j], client);
					j++;
				}
				break;
			case 5:
				flag = 1;
				break;
			case 6:
				flag = 0;
				break;
			default :
				break;
		}
	}
	arg.clear();
}

void	Server::i_mode(int flag, Channel *canal, Client* client)
{
	if (flag == 1)
	{
		std::string mod = SET_channel_MODE(client->getNickname(), client->getUsername(), "MODE", canal->getName(), "i");
		canal->chanmsg(mod, "");
		canal->setInvitation(true);
	}
	else
	{
		std::string mod = UNSET_channel_MODE(client->getNickname(), client->getUsername(), "MODE", canal->getName(), "i");
		canal->chanmsg(mod, "");
		canal->setInvitation(false);
	}
}

void	Server::t_mode(int flag, Channel *canal, Client* client)
{
	if (flag == 1)
	{
		std::string mod = SET_channel_MODE(client->getNickname(), client->getUsername(), "MODE", canal->getName(), "t");
		canal->chanmsg(mod, "");
		canal->setAdmintopic(true);
	}
	else
	{
		std::string mod = UNSET_channel_MODE(client->getNickname(), client->getUsername(), "MODE", canal->getName(), "t");
		canal->chanmsg(mod, "");
		canal->setAdmintopic(false);
	}
}

void	Server::k_mode(Channel *canal, std::string name, Client *client)
{
	std::string mod = SET_channel_MODE(client->getNickname(), client->getUsername(), "MODE", canal->getName(), "k");
	canal->chanmsg(mod, "");
	canal->setPass(true);
	if (!name.empty())
		canal->setPassword(name);
	std::cout << "Channel password: !" << canal->getPassword() << "!" << std::endl;
}

void	Server::k_mode2(Channel *canal, Client *client)
{
	std::string mod = UNSET_channel_MODE(client->getNickname(), client->getUsername(), "MODE", canal->getName(), "k");
	canal->chanmsg(mod, "");
	canal->setPass(false);
}

void    Server::o_mode(int flag, Channel *canal, std::string name, Client *client)
{
    std::map<std::string, Client *> admins = canal->getAdmins();
    std::map<std::string, Client *> clients = canal->getClients();
    std::map<std::string, Client *>::iterator it1 = admins.find(name);
    std::map<std::string, Client *>::iterator it2 = clients.find(name);

    if (flag == 1)
    {
		Client    *user = canal->getIencli(name);

        if (it1 != admins.end())
			return ;
            // throw std::string("MODE: User already an admin !");
        if (it2 == clients.end())
			return ;
            // throw std::string("MODE: User not in the channel !");
        else
		{
			std::string mod = SET_NEWOPER(client->getNickname(), client->getUsername(), "MODE", canal->getName(), "o", user->getNickname());
			canal->chanmsg(mod, "");
            canal->addAdmins(user);
		}
    }
    else
    {
		Client    *user = canal->getIencli(name);
        if (it2 == clients.end())
			return ;
            // throw std::string("MODE: User not in the channel !");
        if (it1 == admins.end())
			return ;
            // throw std::string("MODE: User already not an admin !");
		std::string mod = UNSET_OPER(client->getNickname(), client->getUsername(), "MODE", canal->getName(), "o", user->getNickname());
		canal->chanmsg(mod, "");
        canal->rmAdmins(name);
    }
}

void	Server::l_mode(Channel *canal, std::string name, Client *client)
{
	if (name.empty())
		return ;
		// throw std::string("MODE: you need to specify the limit of user for this channel !");
	int limitusers = atoi(name.c_str());
	if (limitusers == 0)
		return ;
		// throw std::string("MODE : wrong limit of users number !");
	std::string mod = SET_channel_MODE(client->getNickname(), client->getUsername(), "MODE", canal->getName(), "l");
	canal->chanmsg(mod, "");
	canal->setLimituser(true, limitusers);
}

void	Server::l_mode2(Channel *canal, Client *client)
{
	std::string mod = UNSET_channel_MODE(client->getNickname(), client->getUsername(), "MODE", canal->getName(), "l");
	canal->chanmsg(mod, "");
	canal->setLimituser(false, 0);
}
