#include "ChatUI.h"

ChatUI::ChatUI(Application& app, unsigned int num)
{
	this->app = &app;
    chatId = num;
}

void ChatUI::start()
{
    running = true;
    std::thread input_t(&ChatUI::input_thread, this);
    std::thread update_t(&ChatUI::update_thread, this, 5);
    input_t.detach();
    update_t.detach();
}

void ChatUI::stop() 
{
    running = false;
    cv.notify_all();
}

void ChatUI::input_thread() 
{
    while (true)
    {
        std::string tmp;
        if (std::getline(std::cin, tmp)) 
        {
            std::lock_guard<std::mutex> lock(mtx);
            user_input = tmp;
            input_ready = true;
            cv.notify_one(); 
        }
    }
}

void ChatUI::update_thread(int interval_seconds) 
{
    while (true)
    {
        
        std::unique_lock<std::mutex> lock(mtx);

        if (cv.wait_for(lock, std::chrono::seconds(interval_seconds),
            [this] { return this->input_ready.load(); }))
        {
            input_ready = false;

            std::string input = user_input;
            user_input.clear();

            lock.unlock();
            inputHandler(input);
        }
        else
        {
            lock.unlock();
            updateChat();
        }

        std::this_thread::sleep_for(std::chrono::seconds(3));
    }
}


void ChatUI::inputHandler(std::string s)
{
    if (s == "/invite")
    {

    }
    else
        if (s == "/save")
        {

        }
        else app->sendMessage(std::to_string(chatId)),s);
}
void ChatUI::updateChat()
{
    messages.emplace_back(app->getNewMessage(std::to_string(chatId)));
}