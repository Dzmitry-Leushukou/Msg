#include "Message.h"

void Message::setSender(std::string s)
{
	sender = s;
}
std::string Message::getSender()const
{
	return sender;
}
std::string Message::getData()const
{
	return data;
}
std::string Message::getTimestamp()const
{
	return timestamp;
}