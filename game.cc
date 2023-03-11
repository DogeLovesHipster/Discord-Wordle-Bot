#include <vector>
#include <iostream>
#include <unordered_set>
#include <random>
#include <utility>

#include "game.h"

int main()
{

    std::unique_ptr<Dictionary> dictionary = std::make_unique<Dictionary>();

    dictionary = ReadDictionary();

    std::unordered_map<dpp::snowflake, GameState> games;

    const char *token = std::getenv("WORDLE_TOKEN");
    if (!token)
    {
        std::cerr << "Empty token." << std::endl;
        return 1;
    }
    dpp::cluster bot(token);
    bot.on_ready([&bot](const auto &event)
                 { std::cout << "Logged in as " << bot.me.username << "!\n"; });

    bot.on_message_create([&bot, &games, &dictionary](const auto &event)
                          {
                              dpp::snowflake user = event.msg.author.id;
                              if (event.msg.author.username == "wordle-DogeLovesHipster")
                              {
                                  return;
                              }

                              if (event.msg.content == "!play")
                              {
                                  dictionary = ReadDictionary();
                                  bot.message_create(dpp::message(event.msg.channel_id, "Gutentag! I'm yer friendly neighborhood Wordle bot...I mean human. Wordle human. Try to guess the Wordle in __six__ tries..."));
                                  bot.message_create(dpp::message(event.msg.channel_id, "The game is simple, each guess must be valid five-letter word. Hit the enter button to submit."));
                                  bot.message_create(dpp::message(event.msg.channel_id, "After each of your guesses, the color of tiles above will change to show how close you were. \n **Green**: :green_square: is the correct letter and spot. \n **Yellow**: :yellow_square: is the right letter, wrong position. \n **White**: :white_large_square: means the letter is not in the word at all. \n Oh, and you can type __!quit__ to stop the game at any time."));
                                  bot.message_create(dpp::message(event.msg.channel_id, "Please enter a valid five-letter word:"));
                                  InitGame(games, user, dictionary);
                              }

                              else if (event.msg.content == "!quit")
                              {
                                  bot.message_create(dpp::message(event.msg.channel_id, "Auf widenersehen! Going on lunch break... Game ending..."));
                                  games.erase(user);
                              }

                              else
                              {
                                  std::string guess = event.msg.content;
                                  std::string winner = Guess(user, guess, games, dictionary);
                                  games[user].guesses.push_back(winner);

                                  if (winner.find("word") != std::string::npos)
                                  {
                                      bot.message_create(dpp::message(event.msg.channel_id, winner));
                                      games[user].guesses.pop_back();
                                  }

                                  if (games[user].guesses.size() == 6)
                                  {
                                      for (int turns = 0; turns < games[user].guesses.size(); turns++)
                                      {
                                          bot.message_create(dpp::message(event.msg.channel_id, games[user].guesses[turns]));
                                      }
                                      bot.message_create(dpp::message(event.msg.channel_id, "Well, shucks! Seems like you ran out guesses. You lose. The correct word was:"));
                                      bot.message_create(dpp::message(event.msg.channel_id, games[user].word));
                                      bot.message_create(dpp::message(event.msg.channel_id, "Normally, you can only play wordle once in a day, but I'll let you have another go if you type __!play__"));

                                      games.erase(user);
                                  }

                                  else
                                  {
                                      for (int turns = 0; turns < games[user].guesses.size(); turns++)
                                      {
                                          bot.message_create(dpp::message(event.msg.channel_id, games[user].guesses[turns]));
                                      }

                                      int solved = count(winner.begin(), winner.end(), 'g');

                                      if (solved == 5)
                                      {
                                          bot.message_create(dpp::message(event.msg.channel_id, "Woo! You actually solved it. I'm impressed."));
                                          bot.message_create(dpp::message(event.msg.channel_id, "Normally, you can only play wordle once in a day, but I'll let you have another go if you type !play "));

                                          games.erase(user);
                                      }
                                  }
                              }
                          });

    bot.start(false);
    return 0;
}
