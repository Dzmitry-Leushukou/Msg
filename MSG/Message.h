#pragma once

#include <string>
#include <time.h>

class Message
{
public:
	virtual ~Message() = default;
	virtual std::string to_string()=0;
protected:
	std::string sender;
	std::string data;
	time_t last;
};

