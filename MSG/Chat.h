#pragma once
#include "Image.h"
#include "Text.h"
class Chat
{
public:
	Chat(std::vector<std::unique_ptr<Message>>);
	void newMessages(std::vector<std::unique_ptr<Message>>);
	std::vector<std::string> show();
private:
	std::vector<std::unique_ptr<Message>> messages;
};

