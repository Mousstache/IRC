#include "Bot.hpp"

Bot::Bot(Channel *channel)
{
    (void)channel;
}
Bot::~Bot()
{
}

void Bot::ft_bot(int fd)
{
    std::string txt = "PRIVMSG #" + _channel->getName() + " :Je suis là, je ne sert à rien mais je le fais bien toutes les 5 minutes \r\n";
    while (true) {
            send( fd, txt.c_str(), txt.size(), 0);
        sleep(300);
    }
}
