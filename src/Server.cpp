#include "Server.hpp"

#define ERROR_USER(server, username)  "ERROR 42:" + server + "! " + username + " Error with user command" 

Server::~Server(){}

Server::Server(std::string port, std::string password) {
    _socket = -1;
    _bin = -1;
    _lis = -1;
    _socket = socket(AF_INET, SOCK_STREAM, 0);
    _port = atoi(port.c_str());
    _password = password;
    if (_port < 1024 || _port > 65535)
    {
        std::cout << "bad value of port" << std::endl;
        return ;
    }
    if (_socket < 0) {
        std::cerr << "sock error" << std::endl;
        return;
    }
    _sin.sin_family = AF_INET;
    _sin.sin_port = htons(_port);
    _sin.sin_addr.s_addr = INADDR_ANY;

    _bin = bind(_socket, (struct sockaddr *)&_sin, sizeof(_sin));

    if (_bin)
    {
        std::cerr << "bind error" << std::endl;
        return ;
    }
    _lis = listen(_socket, 5);
    if (_lis)
    {
        std::cerr << "listen error" << std::endl;
            return ;
    }
    std::cout << "server is listening" << std::endl;
}

bool controlchar(char c)
{
    return c == '\n' || c == '\r' || c == '\t' || c == '\b' || c == '\a' || c == '\v' || c == '\f';
}

int strncmp(const std::string& str, const std::string& str2) {
    size_t len = str2.size();
    if (str.size() < len)
        return -1;
    for (size_t i = 0; i < len; ++i)
    {
        if (str[i] != str2[i])
            return 0;
    }
    return len;
}

int cmdparser(const std::string& str) {
    if (strncmp(str, "PASS"))
        return 4;
    if (strncmp(str, "NICK"))
        return 4;
    if (strncmp(str, "USER"))
        return 4;
    if (strncmp(str, "JOIN"))
        return 4;
    return 0;
}

void Server::exec_cmd(std::string const &command, std::string const &value, std::vector<Client>::iterator it)
{
    if (command == "PASS")
    {
        it->setPassword(value);
        it->_check++;
    } else if (command == "NICK")
    {
        it->setNickname(value);
        it->_check++;
    } else if (command == "USER")
    {
        it->setUsername(value);
        it->_check++;
    }

}

bool Server::cmd_pars(std::vector<Client>::iterator it, std::string buffer)
{
    size_t i = 0;
    size_t cmdStart = buffer.find_first_of("JOIN", i);
    std::string passvalue;
    if (cmdStart == std::string::npos)
        return (false);
    size_t cmdEnd = buffer.find_first_of(" ", cmdStart);
    if (cmdEnd == std::string::npos) 
        return (false);
    std::string command = buffer.substr(cmdStart, cmdEnd - cmdStart);
    std::cout << "Command: " << command << std::endl;
    size_t valueStart = buffer.find_first_not_of(" #", cmdEnd);
    if (valueStart == std::string::npos)
        return (false);
    size_t valueEnd = buffer.find_first_of(" \r\n", valueStart);
    if (valueEnd == std::string::npos)
        return (false);
    std::string value = buffer.substr(valueStart, valueEnd - valueStart);
    std::cout << "Value: " << value << std::endl;
    if (command != "JOIN")
        return (false);
    size_t pass = buffer.find_first_not_of(" ", valueEnd);
    size_t passend = buffer.find_first_of(" \r\n", pass);
    // if (passend == std::string::npos || pass == std::string::npos)
    //     joinChannel(value, &(*it), false);
    if(passend != std::string::npos && pass != std::string::npos)
    {
        std::string passvalue = buffer.substr(pass, passend - valueStart);
        std::cout << "pass: " << passvalue << std::endl;
    }
    joinChannel(value, passvalue, &(*it), false);
    return (true);
}

void Server::set_id(std::string& str, std::vector<Client>::iterator it) {
    size_t i = 0;

    while (i < str.size()) {
        size_t cmdStart = str.find_first_of("PASS, NICK, USER", i);
        if (cmdStart == std::string::npos)
            break;
        size_t cmdEnd = str.find_first_of(" \r\n", cmdStart);
        if (cmdEnd == std::string::npos) 
            break;
        std::string command = str.substr(cmdStart, cmdEnd - cmdStart);
        std::cout << "Command: " << command << std::endl;
        size_t valueStart = str.find_first_not_of(" \r\n", cmdEnd);
        if (valueStart == std::string::npos)
            break;
        size_t valueEnd = str.find_first_of(" \r\n", valueStart);
        if (valueEnd == std::string::npos)
            break;
        std::string value = str.substr(valueStart, valueEnd - valueStart);
        std::cout << "Value: " << value << std::endl;
        if (cmdparser(command) == 4) {
             exec_cmd(command, value, it);
        }
        i = valueEnd + 2;
    }
}


bool Server::parse(std::vector<Client>::iterator it, std::string buffer)
{
    if(buffer[strlen(buffer.c_str()) - 1 ] == '\n' && buffer[strlen(buffer.c_str()) - 2 ] != '\r')
    {
        if(!it->getRegister())
        {
            set_id(buffer, it);
            std::cout << "==============================" << std::endl;
            std::cout << "[ PASS = "<< it->getPassword() << " USER = " << it->getUsername() << " NICK = " << it->getNickname() << " ]" <<  std::endl;
            std::cout << "==============================" << std::endl;
            if (it->_check == 3)
            {
                std::string welcome = "Welcome to the IRC Network\n";
                send(it->socket_fd, welcome.c_str(), welcome.size(), 0);
                it->setRegister(true);
            }
            return (true);
        }
    }
    else if(buffer[strlen(buffer.c_str()) - 1 ] == '\n' && buffer[strlen(buffer.c_str()) - 2 ] == '\r')
    {
        if(!it->getRegister())
        {
            set_id(buffer, it);
            std::cout << "==============================" << std::endl;
            std::cout << "[ PASS = "<< it->getPassword() << " USER = " << it->getUsername() << " NICK = " << it->getNickname() << " ]" <<  std::endl;
            std::cout << "==============================" << std::endl;
            if (it->_check == 3)
            {
                it->setRegister(true);
                std::string welcome = ":irc.example.com 001 :Welcome to the IRC Network \r\n";
                send(it->socket_fd, welcome.c_str(), welcome.size(), 0);
            }
        }
        else if (it->getRegister())
        {
            if (cmd_pars(it, buffer))
            {
                std::cout << "ok" << std::endl;
            }
            else
            {
                buffer.clear();
            }
        }

        return(true);
    }
    std::cout << "Erreur buf"<< std::endl;
    return (false);
}


void Server::serving() {
    fd_set rfds, tmp_rfds;
    int max_sd, new_socket;
    struct sockaddr_in client;
    socklen_t addr_len = sizeof(client);
    std::vector<Client> clients;

    FD_ZERO(&rfds);
    FD_SET(_socket, &rfds);
    max_sd = _socket;

    std::cout << "Server is waiting for connections on port " << _port << std::endl;

    while (true)
    {
        tmp_rfds = rfds;

        if (select(max_sd + 1, &tmp_rfds, NULL, NULL, NULL) == -1) {
            std::cerr << "select error" << std::endl;
            break;
        }

        if (FD_ISSET(_socket, &tmp_rfds)) {
            if ((new_socket = accept(_socket, (struct sockaddr *)&client, &addr_len)) == -1) {
                std::cerr << "accept error" << std::endl;
                break;
            } else {
                std::cout << "New connection from " << inet_ntoa(client.sin_addr) << " on socket " << new_socket << std::endl;
                clients.push_back(Client(new_socket, client));
                FD_SET(new_socket, &rfds);
                if (new_socket > max_sd)
                    max_sd = new_socket;
            }
        }
        for (std::vector<Client>::iterator it = clients.begin(); it != clients.end();)
        {
            int client_socket = it->socket_fd;
            if (FD_ISSET(client_socket, &tmp_rfds)) {
                char buffer[1024];
                int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
                if (bytes_received <= 0)
                {
                    std::cout << "Client on socket " << client_socket << " " << it->getUsername() << " disconnected." << std::endl;
                    close(client_socket);
                    FD_CLR(client_socket, &rfds);
                    it = clients.erase(it);
                }
                else
                {
                    buffer[bytes_received] = '\0';
                    std::cout << std::endl;
                    parse(it, buffer);
                    std::cout << "Received from client " << client_socket << ": [" << buffer << "]" << std::endl;
                    ++it;
                }
            } else
            {
                ++it;
            }
        }
    }
    close(_socket);
}

bool Server::channelExists(const std::string& name) const
{
    return channels.find(name) != channels.end();
}

void Server::joinChannel(const std::string& channelName, const std::string& pass, Client* client, bool isAdmin)
{
    if (!channelExists(channelName)) {
        Channel* newChannel = new Channel(channelName, client);
        if (!pass.empty()) {
            newChannel->setPassword(pass);
        }
        channels[channelName] = newChannel;
    } else {
        if (!pass.empty() && channels[channelName]->neededPassword() && pass != channels[channelName]->getPassword()) {
            std::string error_passwd = "Error password\r\n";
            send(client->socket_fd, error_passwd.c_str(), error_passwd.size(), 0);
            return;
        }
    }
    channels[channelName]->addClient(client);
    if (isAdmin) {
        channels[channelName]->setAdmin(client->getNickname());
    }
    channels[channelName]->print();
}



                    // for (size_t i = 0; buffer[i]; i++)
                    // {
                    //     if (buffer[i] == '\n')
                    //     {
                    //         std::cout << "[\\n]" ;
                    //     }
                    //     else if (buffer[i] == '\r')
                    //     {
                    //         std::cout << "[\\r]" ;
                    //     }
                    //     else if (buffer[i] == ' ')
                    //         std::cout << "[ ]" ;
                    //     else
                    //         std::cout << buffer[i];
                    // }