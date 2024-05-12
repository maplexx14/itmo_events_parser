#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <string>
#include "src/get_data.cpp"
#include <tgbot/tgbot.h>

using namespace std;
using namespace TgBot;

int bot_main() {
    string token("6220483217:AAEAz0WiEc8Ujw6KMO7a_np4WrL8EwNaT7U");
    printf("Token: %s\n", token.c_str());

    Bot bot(token);

    InlineKeyboardMarkup::Ptr keyboard(new InlineKeyboardMarkup);
    vector<InlineKeyboardButton::Ptr> row0;
    InlineKeyboardButton::Ptr checkButton(new InlineKeyboardButton);
    checkButton->text = "Получить расписание";
    checkButton->callbackData = "check";
    row0.push_back(checkButton);
    keyboard->inlineKeyboard.push_back(row0);


 

    InlineKeyboardButton::Ptr forwardButton(new InlineKeyboardButton);
    forwardButton->text = "Вперед";
    forwardButton->callbackData = "forward";
   
      
   

    bot.getEvents().onCommand("start", [&bot, &keyboard](Message::Ptr message) {
        bot.getApi().sendMessage(message->chat->id, "Привет нажми на кнопку, чтобы узнать расписание всех мероприятий в ИТМО", false, 0, keyboard);
        });
    bot.getEvents().onCallbackQuery([&bot, &keyboard](CallbackQuery::Ptr query) {
        if (StringTools::startsWith(query->data, "back")) {
            bot.getApi().deleteMessage(query->message->chat->id, query->message->messageId);
        }
        });
    bot.getEvents().onCallbackQuery([&bot, &keyboard](CallbackQuery::Ptr query) {
        if (StringTools::startsWith(query->data, "check")) {
            InlineKeyboardMarkup::Ptr back_keyboard(new InlineKeyboardMarkup);
            vector<InlineKeyboardButton::Ptr> back_row0;
            InlineKeyboardButton::Ptr backButton(new InlineKeyboardButton);
            backButton->text = "Назад";
            backButton->callbackData = "back";
            back_row0.push_back(backButton);
            back_keyboard->inlineKeyboard.push_back({ backButton });
            get_data_site();
            ifstream ifs("data.json");
            string response = "ok";
            bot.getApi().sendMessage(query->message->chat->id, response, false, 0, back_keyboard);
        }
        });

    signal(SIGINT, [](int s) {
        printf("SIGINT got\n");
        exit(0);
        });

    try {
        printf("Bot username: %s\n", bot.getApi().getMe()->username.c_str());
        bot.getApi().deleteWebhook();

        TgLongPoll longPoll(bot);
        while (true) {
            printf("Long poll started\n");
            longPoll.start();
        }
    }
    catch (exception& e) {
        printf("error: %s\n", e.what());
    }

    return 0;
}