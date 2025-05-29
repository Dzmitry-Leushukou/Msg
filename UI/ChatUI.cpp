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
    updateChatUI();
    running = true;

    update_thread_future = std::async(std::launch::async, &ChatUI::update_thread, this, 3);

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

        if (input == "/q") {
            stop();
            return;
        }

        inputHandler(input);
    }
}

void ChatUI::update_thread(int interval_seconds)
{
    while (running)
    {
        {
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait_for(lock, std::chrono::seconds(interval_seconds),
                [this] { return !running; });

            if (!running) break;
        }

        updateChat();
    }
}

void ChatUI::inputHandler(const std::string& s)
{
    if (s == "/invite") {

    }
    else if (s == "/save") {
    }
    if (s.size() > 5 && s.substr(0, 5) == "/load")
    {
        //load image
    }
    else {
        // Отправка сообщения
        // app.sendMessage(std::to_string(chatId), s);
        updateChatUI();
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

    // Обновляем UI
    updateChatUI();
}

void ChatUI::updateChatUI()
{
    std::lock_guard<std::mutex> lock(mtx);

    clearScreen();
    std::cout << "===" << app->getCurChatName() << "===\n\n";

    for (auto& msg : messages) {
        std::cout << msg->to_string() << '\n';
    }

    std::cout << "\nWrite message (/q - exit): ";
    std::flush(std::cout);
}