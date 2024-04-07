#ifndef SERVER_HPP
#define SERVER_HPP
#pragma once

#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h> 
#include <cstdio>
#include <cstdlib>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <map>
#include <string>
#include <iostream>
#include <vector>
#include <sstream>
#include <memory>
#include "Client.hpp"
#include "Channel.hpp"

# define ERR_NOTONCHANNEL(nickname, channel) (":localhost 442 " + nickname + " #" + channel + " :You're not on that channel\r\n")
# define ERR_NOSUCHCHANNEL(nickname, channel) (":localhost 403 " + nickname + " " + channel + " :No such channel\r\n") //
# define ERR_USERNOTINCHANNEL(nickname, concerned_client_nickname, channel) ((":localhost 441 " + nickname + " " + concerned_client_nickname + " " + channel + " :They aren't on that channel\r\n"))
# define ERR_USERONCHANNEL(nickname, channel) (":localhost 443 " + channel + " " + nickname + " :is already on channel\r\n") //
# define ERR_CHANOPRIVSNEED(nickname, channel) (":localhost 482 " + nickname + " #" + channel + " :You're not channel operator\r\n")
#define KICK_CLIENT(nickname, username, cmd, channel, concerned_client_nickname)((CLIENT_ID(nickname, username, cmd)) + channel + " " + concerned_client_nickname + " :\r\n")

# define CLIENT_ID(nickname, username, command) (":" + nickname + "!~" + username + "@" + "mat" + " " + command + " ")

# define RPL_PRIVMSG_CLIENT(nickname, username, dest, msg) (nickname, username, "PRIVMSG " + dest + " :" + msg + "\r\n")
# define RPL_PRIVMSG_CHANNEL(nickname, channel, msg) (":" + nickname + " PRIVMSG "  + channel + " " + msg + "\r\n") //used

// # define INVITE_CLIENT(nickname, username, cmd, concerned_client_nickname, channel) (user_id(nickname, username, cmd) + concerned_client_nickname + " :" + channel + "\r\n") //

# define ERR_USERONCHANNEL(nickname, channel) (":localhost 443 " + channel + " " + nickname + " :is already on channel\r\n")
# define RPL_INVITING(nickname, target, channel) (":localhost 341 " + nickname + " " + target + " :" + channel + "\r\n") 
# define INVITE_CLIENT(nickname, username, cmd, concerned_client_nickname, channel) (nickname, username, cmd + concerned_client_nickname + " :" + "#" + channel + "\r\n") //
# define NOTICE_CLIENT_INVITE(nickname, channel) (nickname + " invites you to " + channel + "\r\n") //


#define RPL_NOTOPIC(nickname, channel) (":localhost 331 " + nickname + " #" + channel + " :No topic is set\r\n") //
#define RPL_TOPIC(nickname, channel, topic) (":localhost 332 " + nickname + " #" + channel + " : " + topic + "\r\n") //
#define RPL_TOPICWHOTIME(nickname, channel, nickname_who, time) (":localhost 333 " + nickname + " #" + channel + " " + nickname_who + " " + time + "\r\n")


extern bool	server_off;

class Client;
class Channel;

class Server {
    public:
        Server(std::string port, std::string password);
        Server(int port);
        ~Server();
        void serving();
        void startListen();
        void serve();
        std::string getPassword();
        void printserv();
        Client *getClient(std::string name);
        Channel *getChannel(std::string channelname);
        void exitWithError(std::string errorMessage);
        void log(const std::string& message);
        void parse(Client *client, std::string buffer);

        void set_id(std::string str, Client *client);
        void exec_cmd(std::string const &command, std::string const &value, Client *client);
        void cmd_pars(Client *client,std::string buffer);
        bool joinChannel(Client *client, std::string channelname, std::string password);

        bool join_pars(Client *client,std::string buffer);
        bool ping_pars(Client *client, std::string buffer);
        void kick_pars(Client *client, std::string buffer);
        void kick_exec(Client *client, std::string channel, std::string name, std::string reason);
        void invite_pars(Client *client, std::string buffer);
        void invite_exec(Client *client, std::string user, std::string channel);
        void mode_pars(Client *client, std::string buffer);
        void mode_exec(Client *client, std::string channel, std::string mode, std::string name);
        void mode_option(Client *client, std::string mode, std::string name, Channel *canal);
        int check_option(const std::string &mode);
        void topic_pars(Client *client,std::string buffer);
        void topic_exec(Client *client, std::string channel, std::string newTopic);
        void privmsg_pars(Client *client, std::string buffer);
        void privmsg_exec(Client *client, std::string user, std::string msg);
        void part_pars(Client *client, std::string buffer);
        void part_exec(Client *client, std::string channel);
        int which_command(std::string cmd);

        void i_mode(std::string mode, Channel *canal);
        void t_mode(std::string mode, Channel *canal);
        void k_mode(std::string mode, Channel *canal);
        void o_mode(std::string mode, Channel *canal, std::string name);
        void l_mode(std::string mode, Channel *canal, std::string name);
    private:
        std::string _password;
        int _socket;
        struct sockaddr_in _sin;
        int _bin;
        int _lis;
        int _port;
        void handleNewConnection();
        void handleMessage(int client_socket, sockaddr_in newSockAddr);
        void handleDisconnection(int client_socket);
        std::map<std::string , Channel *> _channel;
        std::map<int , Client *>  _client;
        bool _server_off;
};

#endif