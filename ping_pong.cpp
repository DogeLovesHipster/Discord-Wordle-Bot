#include <dpp/dpp.h>
#include <iostream>
using namespace std;

// Simple code that allows the bot to send a pong message to your /ping
int main()
{
    const char *token = std::getenv("TOKEN");
    if (!token)
    {
        cerr << "Empty token." << endl;
        return 1;
    }
    dpp::cluster bot(token);

    bot.on_ready([&bot](const auto &event)
                 { std::cout << "Logged in as " << bot.me.username << "!\n"; });

    bot.on_message_create([&bot](const auto &event)
                          {
        if (event.msg.content == "/ping") {
            bot.message_create(dpp::message(event.msg.channel_id, "Pong!"));
        } });

    bot.start(false);
    return 0;
}
