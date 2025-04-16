#include "Chat.h"

void Chat::newMessages(std::vector<std::unique_ptr<Message>>nm)
{
	for (auto& msg : nm)
		messages.push_back(std::move(msg));
}

void Chat::show()
{
	for (auto& i : messages)
	{
		std::cout << i->to_string() << '\n';
	}
}

Chat::Chat(std::vector<std::unique_ptr<Message>> m	)
{
	messages = std::move(m);
}

