#pragma once
#include "Page.h"
#include "Message.h"
#include <iostream>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <string>
class ChatUI :
    public Page
{
public:
    ChatUI(Application& app, unsigned int num);
    virtual void start() override;
    void stop();
private:
    void input_thread();
    void inputHandler(std::string s);
    void updateChat();
    void update_thread(int interval_seconds);
    std::atomic<bool> input_ready{ false };
    std::atomic<bool> running{ true };
    std::string user_input;
    std::mutex mtx;
    std::condition_variable cv;
    std::vector<std::unique_ptr<Message>>messages;
    time_t lastUpdateTime=0;
};

