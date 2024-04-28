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
#include <ctime>


# define RPL_PONG "PONG :irc.example.com@localhost\r\n"

 #define  NICK_RPL(nickname, username, new_nickname) (CLIENT_ID(nickname, username, "NICK") + ":" + new_nickname + "\r\n")


# define ERR_CHANNELISFULL(nickname, channel) (":localhost 471 " + nickname + " " + channel + " :Cannot join channel (+l)\r\n") //

# define RPL_NAMREPLY(nickname, channel, list_client) (":localhost 353 " + nickname + " = " + channel + " :" + list_client +"\r\n") //

# define RPL_ENDOFNAMES(nickname, channel) (":localhost 366 " + nickname + " " + channel + " :End of /NAMES list.\r\n")//

# define SET_channel_MODE(nickname, username, cmd, channel, mode) (user_id(nickname, username, cmd) + "#" + channel + " +" + mode + "\r\n")
# define UNSET_channel_MODE(nickname, username, cmd, channel, mode) (user_id(nickname, username, cmd) + "#" +channel + " -" + mode + "\r\n") //
# define SET_NEWOPER(nickname, username, cmd, channel, mode, concerned_client_nickname) ((user_id(nickname, username, cmd) + "#" + channel + " +" + mode  + " " + concerned_client_nickname + "\r\n")) //
# define UNSET_OPER(nickname, username, cmd, channel, mode, concerned_client_nickname) ((user_id(nickname, username, cmd) + "#" + channel + " -" + mode  + " " + concerned_client_nickname + "\r\n")) //

# define ERR_NOSUCHNICK(nickname, wrong_nick) (":localhost 401: " + nickname + " There is no such nick : " + wrong_nick + "\r\n") //
# define ERR_NOTONCHANNEL(nickname, channel) (":localhost 442 " + nickname + " #" + channel + " :You're not on that channel\r\n")
# define ERR_NOSUCHCHANNEL(nickname, channel) (":localhost 403 " + nickname + " " + channel + " :No such channel\r\n") //
# define ERR_USERNOTINCHANNEL(nickname, concerned_client_nickname, channel) ((":localhost 441 " + nickname + " " + concerned_client_nickname + " " + channel + " :They aren't on that channel\r\n"))
# define ERR_USERONCHANNEL(nickname, channel) (":localhost 443 " + channel + " " + nickname + " :is already on channel\r\n") //
# define ERR_CHANOPRIVSNEED(nickname, channel) (":localhost 482 " + nickname + " #" + channel + " :You're not channel operator\r\n")

# define PART_channel(nickname, username, cmd, channel) (user_id(nickname, username, cmd) + channel + "\r\n") //

# define KICK_CLIENT(nickname, username, cmd, channel, concerned_client_nickname)((user_id(nickname, username, cmd)) + "#" + channel + " " + concerned_client_nickname + " :\r\n")

# define KICK_CLIENT_REASON(nickname, username, cmd, channel, concerned_client_nickname, reason)((user_id(nickname, username, cmd)) + "#" + channel + " " + concerned_client_nickname + " :" + reason + "\r\n")

// # define CLIENT_ID(nickname, username, command) (":" + nickname + "!~" + username + "@" + "eliotalderson" + " " + command + " ")
# define CLIENT_ID(nickname, username, command) (":" + nickname + "!~" + username + "@" + "localhost" + " " + command + " ")
# define user_id(nickname, username, cmd) (":" + nickname + "!~" + username + "@localhost " + cmd + " ") //

// # define RPL_PRIVMSG_CLIENT(nickname, username,  dest, msg) (":" + nickname + username , "PRIVMSG " + dest + " :" + msg + "\r\n")
# define RPL_PRIVMSG_CLIENT(nickname, username, dest, msg) (CLIENT_ID(nickname, username, "PRIVMSG") + dest + " :" + msg + "\r\n") //used
# define RPL_PRIVMSG_CHANNEL(nickname, channel, msg) (":" + nickname + " PRIVMSG "  + channel + " " + msg + "\r\n") //used
# define RPL_PRIVMSG_CHANNEL_CMP(nickname, channel, msg) (":" + nickname + " PRIVMSG " + "#" + channel + " " + msg + "\r\n")
# define ERR_NOTEXTTOSEND(nickname) (nickname + " :No text to send\r\n") 

# define NOTICE_CLIENT_INVITE(nickname) (nickname + " cannot kick " + "\r\n") //

# define ERR_INVITEONLYCHAN(nickname, channel) (":localhost 473 " +  nickname + " " + channel + " :Cannot join channel (+i)\r\n") //
# define INVITE_CLIENT(nickname, username, cmd, concerned_client_nickname, channel) (CLIENT_ID(nickname, username, cmd) + concerned_client_nickname + " :" + channel + "\r\n") //

# define ERR_USERONCHANNEL(nickname, channel) (":localhost 443 " + channel + " " + nickname + " :is already on channel\r\n")
# define RPL_INVITING(nickname, target, channel) (":localhost 341 " + nickname + " " + target + " :" + channel + "\r\n") 
// # define INVITE_CLIENT(nickname, username, cmd, concerned_client_nickname, channel) (nickname, username, cmd + concerned_client_nickname + " :" + "#" + channel + "\r\n") //
// # define NOTICE_CLIENT_INVITE(nickname, channel) (nickname + " invites you to " + channel + "\r\n") //

// # define ERR_NICKNAMEINUSE(username, nickname) (":localhost 433 " + username + " " + nickname + " :Nickname is already in use\r\n")
# define ERR_NICKNAMEINUSE(nickname) (":localhost 433 * " + nickname + " :Nickname is already in use\r\n")
# define RPL_NOTOPIC(nickname, channel) (":localhost 331 " + nickname + " #" + channel + " :No topic is set\r\n") //
# define RPL_TOPIC(nickname, channel, topic) (":localhost 332 " + nickname + " #" + channel + " : " + topic + "\r\n") //
# define RPL_TOPICWHOTIME(nickname, channel, nickname_who, time) (":localhost 333 " + nickname + " #" + channel + " " + nickname_who + " " + time + "\r\n")
# define RPL_CREATION_TIME(nickname, channel, time) (":localhost 329 " + nickname + " #" + channel + " " + time + "\r\n")

# define ERR_BADCHANNELKEY(nickname, channel) (":localhost 475 " + nickname + " " + channel + " :\r\n") //

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
        void joinChannel(Client *client, std::string channelname, std::string password);

        void nick_pars(Client *client, std::string buffer);
        void nick_exec(Client *client, std::string user);
        void join_pars(Client *client,std::string buffer);
        bool ping_pars(Client *client, std::string buffer);
        void kick_pars(Client *client, std::string buffer);
        void kick_exec(Client *client, std::string channel, std::string name, std::string reason);
        void invite_pars(Client *client, std::string buffer);
        void invite_exec(Client *client, std::string user, std::string channel);
        void mode_pars(Client *client, std::string buffer);
        void mode_exec(Client *client, std::string channel, std::string mode, std::vector<std::string> arg);
        void mode_option(Client *client, std::string mode, std::vector<std::string> arg, Channel *canal);
        int  check_option(char option);
        void topic_pars(Client *client,std::string buffer);
        void topic_exec(Client *client, std::string channel, std::string newTopic);
        void privmsg_pars(Client *client, std::string buffer);
        void privmsg_exec(Client *client, std::string user, std::string msg);
        void part_pars(Client *client, std::string buffer);
        void part_exec(Client *client, std::string channel);
        int which_command(std::string cmd);

        void i_mode(int flag, Channel *canal, Client* client);
        void t_mode(int flag, Channel *canal, Client* client);
        void k_mode(Channel *canal, std::string name, Client *client);
        void k_mode2(Channel *canal, Client *client);
        void o_mode(int flag, Channel *canal, std::string name, Client *client);
        void l_mode(Channel *canal, std::string name, Client* client);
        void l_mode2(Channel *canal, Client* client);
    private:
        std::string _password;
        std::string _old_buf;
        int _socket;
        struct sockaddr_in _sin;
        int _bin;
        int _lis;
        int _port;
        int index;
        void handleNewConnection();
        void handleMessage(int client_socket, sockaddr_in newSockAddr);
        void handleDisconnection(int client_socket);
        std::map<std::string , Channel *> _channel;
        std::map<int, Client *>  _client;
        bool _server_off;
};

#endif
