#pragma once

#include <string>
#include <chrono>

class Message
{
public:
	virtual ~Message() = default;
	virtual std::string to_string()=0;
protected:
	std::string sender;
	std::string data;
	std::string timestamp;
};

