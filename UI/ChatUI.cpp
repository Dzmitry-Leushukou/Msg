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

        if (input == "q")
        {
            stop();
            return;
        }else
        if (input == "s")
        {
            pauseUpdates();
            sendMessage();
            resumeUpdates();
            updateChat();
        }
        else
        if (input == "i")
        {
            pauseUpdates();
            sendImage();
            resumeUpdates();
        }
        else
        if (input == "+")
        {

            pauseUpdates();
            sendInvite();
            resumeUpdates();
            updateChat();
        }
        else
            if (input == "o")
            {
                pauseUpdates();
                openImage();
                resumeUpdates();
                updateChat();
            }
        else
        {
            std::cout << "Wrong input. Try again\n";
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
    int id = 0;
    for (auto& msg : messages) 
    {
        Image* image_msg = dynamic_cast<Image*>(msg.get());
        if (image_msg)
            std::cout << "[id = " << id << "]\n",id++;
        std::cout << msg->to_string() << '\n';
    }
    imagesCount = id;
    std::cout << "=============================\nq - exit\ns - write text message\ni - sent image\no - open image\n+ - send invite to other user\nWrite type of opreation: ";
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
    std::getline(std::cin, s);
    try
    {
        app->sendInvite(s);
    }
    catch (std::exception& e)
    {
        std::cout << e.what()<<"\n";
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
        app->sendImage(s);
    }
    catch (...)
    {
        std::cout << "Something went wrong. Try again\n";
        pause();
    }
}

void ChatUI::openImage()
{
    int id = getInt("To open image write id of message with it(-1 to exit): ",-1, imagesCount - 1);
    if (id == -1)
        return;
    std::string data;
    std::string format;
    for (auto& msg : messages)
    {
        Image* image_msg = dynamic_cast<Image*>(msg.get());
        if (image_msg)
        {
            id--;
            if (id < 0)
            {
                data = msg->getData();
                format = image_msg->getFormat();
                std::string path = FileService::saveImage(data, format);
                std::cout << "Image save to " << path << '\n';
                std::string cmd = "start " + path;
                system(cmd.c_str());
                system("pause");
                return;
            }
        }
    }

}