#pragma once
#include "Page.h"
#include "Text.h"
#include "Image.h"
#include <iostream>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <string>
#include <future> 
#include <memory>
class ChatUI :
    public Page
{
public:
    ChatUI(Application& app, unsigned int num);
    virtual ~ChatUI();

    virtual void start() override;
    void stop();
private:
    void input_thread();
    void update_thread(int interval_seconds);
    void inputHandler(const std::string& s);
    void updateChat();
    void updateChatUI();
    void pauseUpdates();
    void resumeUpdates();
    void sendMessage();
    void sendInvite();
    void sendImage();
    void openImage();
    std::condition_variable pause_cv;
    std::atomic<bool> updatesPaused{ false };

    unsigned int chatId;

    std::atomic<bool> running{ true };
    std::mutex mtx;
    std::condition_variable cv;

    std::vector<std::unique_ptr<Message>> messages;
    time_t lastUpdateTime = 0;
    int imagesCount = 0;
    std::future<void> update_thread_future;
};

