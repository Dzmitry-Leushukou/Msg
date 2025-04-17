#include "Chat.h"

void Chat::newMessages(std::vector<std::unique_ptr<Message>>nm)
{
	for (auto& msg : nm)
		messages.push_back(std::move(msg));
}

std::vector<std::string> Chat::show()
{
	std::vector<std::string> res;
	for (auto& i : messages)
	{
		res.push_back(i->to_string() +"\n");
	}
	return res;
}

Chat::Chat(std::vector<std::unique_ptr<Message>> m)
{
	messages = std::move(m);
}

