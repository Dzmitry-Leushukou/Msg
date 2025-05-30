#pragma once
#ifdef MSG_EXPORTS
#define EXP __declspec(dllexport)
#else
#define EXP __declspec(dllimport)
#endif
#include <string>
#include <chrono>

class EXP Message
{
public:
	virtual ~Message() = default;
	virtual std::string to_string()=0;
	void setSender(std::string);
	std::string getSender()const;
	std::string getData()const;
	std::string getTimestamp()const;
protected:
	std::string sender;
	std::string data;
	std::string timestamp;
};

