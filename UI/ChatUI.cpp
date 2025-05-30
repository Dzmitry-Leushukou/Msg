#include "ChatUI.h"

ChatUI::ChatUI(Application& app, unsigned int num)
{
	this->app = &app;
    this->app->loadChat(num);
}
ChatUI::~ChatUI()
{
    stop();
}

void ChatUI::start()
{
    updateChat();
    running = true;

    update_thread_future = std::async(std::launch::async, &ChatUI::update_thread, this, 2);

    input_thread();
}

void ChatUI::stop()
{
    running = false;
    cv.notify_all();

    if (update_thread_future.valid()) {
        update_thread_future.get();
    }
}

void ChatUI::input_thread()
{
    while (running)
    {
        std::string input;
        std::getline(std::cin, input);

        if (input == "q")
        {
            stop();
            return;
        }
        if (input == "s")
        {
            pauseUpdates();
            sendMessage();
            resumeUpdates();
            updateChat();
        }
        if (input == "i")
        {
            pauseUpdates();
            sendImage();
            resumeUpdates();
        }
        if (input == "+")
        {

            pauseUpdates();
            sendInvite();
            resumeUpdates();
            updateChat();
        }
    }
}

void ChatUI::update_thread(int interval_seconds)
{
    while (running)
    {
        std::unique_lock<std::mutex> lock(mtx);

        pause_cv.wait(lock, [this] {
            return !updatesPaused.load() || !running;
            });

        if (!running) break;

        cv.wait_for(lock, std::chrono::seconds(interval_seconds),
            [this] { return !running || updatesPaused; });

        if (!running || updatesPaused) continue;

        lock.unlock();
        updateChat();
    }

}


void ChatUI::updateChat()
{
    auto new_messages = app->getNewMessage(lastUpdateTime);
    if (new_messages.empty()) return;
    lastUpdateTime = time(0);
    {
        std::lock_guard<std::mutex> lock(mtx);
        for (auto& msg : new_messages) {
            messages.push_back(std::move(msg));
        }
        
    }
    updateChatUI();
}

void ChatUI::updateChatUI()
{
    std::lock_guard<std::mutex> lock(mtx);

    clearScreen();
    std::cout << "===" << app->getCurChatName() << "===\n\n";

    for (auto& msg : messages) {
        std::cout << msg->to_string();
    }

    std::cout << "=============================\nq - exit\ns - write text message\ni - sent image\n+ - send invite to other user\nWrite type of opreation: ";
    std::flush(std::cout);
}

void ChatUI::pauseUpdates()
{
    std::lock_guard<std::mutex> lock(mtx);
    updatesPaused = true;
}

void ChatUI::resumeUpdates()
{
    {
        std::lock_guard<std::mutex> lock(mtx);
        updatesPaused = false;
    }
    pause_cv.notify_one();  
    updateChatUI();        
    //std::cout << "\n[Обновления возобновлены]\n";
}

void ChatUI::sendMessage()
{
    std::cout << "> ";
    std::string s;
    std::string sender = "unknown";
    std::getline(std::cin, s);
    
    app->sendMessage(std::make_unique<Text>(sender, s, std::to_string(time(0))));
}
void ChatUI::sendInvite()
{
    std::cout << "Write username of reciever: ";
    std::string s;
    try
    {
        //app->sendInvite(s);
    }
    catch (...)
    {
        std::cout << "Something went wrong. Try again\n";
        pause();
    }
}
void ChatUI::sendImage()
{
    std::string s;
    std::cout << "Write filepath: ";
    std::getline(std::cin, s);
    
    try 
    {
        //app->sendImage(s);
    }
    catch (...)
    {
        std::cout << "Something went wrong. Try again\n";
        pause();
    }
}