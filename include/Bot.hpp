#ifndef BOT_HPP
#define BOT_HPP

#include <iostream>
#include <ctime>
#include "Channel.hpp"

class Channel;

class Bot {
    public:
        Bot(Channel *channel);
        ~Bot();
        void    ft_bot(int fd);
    private:
        Channel * _channel;
};

#endif