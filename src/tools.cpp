#include "../includes/server.hpp"

std::vector<std::string> splitByComma(const std::string &s)
{
	std::vector<std::string> result;
	std::string current;
	for (size_t i = 0; i < s.size(); ++i)
	{
		if (s[i] == ',')
		{
			if (!current.empty())
				result.push_back(current);
			current.clear();
		}
		else
			current += s[i];
	}
	if (!current.empty())
		result.push_back(current);
	return result;
}

namespace
{
	bool hasOnlyDigits(const std::string &s)
	{
		if (s.empty())
			return false;
		for (size_t i = 0; i < s.size(); ++i)
		{
			if (s[i] < '0' || s[i] > '9')
				return false;
		}
		return true;
	}
}


int parsePort(const std::string &port)
{
	if (!hasOnlyDigits(port))
		throw std::invalid_argument("Wrong Port\n");

	long value = 0;
	for (size_t i = 0; i < port.size(); ++i)
	{
		value = value * 10 + (port[i] - '0');
		if (value > 65535)
			throw std::invalid_argument("The Number is Above the Port limits\n");
	}

	if (value < 1024)
		throw std::invalid_argument("Wrong Port\n");
	return (int)(value);
}

bool passwordHasNoWhitespace(const std::string &passwd)
{
	for (size_t i = 0; i < passwd.size(); i++)
	{
		if ((passwd[i] >= 9 && passwd[i] <= 13) || passwd[i] == ' ')
			return false;
	}
	return true;
}