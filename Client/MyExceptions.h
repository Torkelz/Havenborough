#pragma once

#include <exception>
#include <string>

class MyException : public std::exception
{
protected:
	std::string m_What;
	int m_Line;
	std::string m_File;

public:
	MyException(const std::string& p_What, int p_Line, const std::string& p_File)
		: m_What(p_What),
		  m_Line(p_Line),
		  m_File(p_File)
	{
	}

	virtual const char* what() const throw() override
	{
		return (m_File + ":" + std::to_string(m_Line) + ": " + m_What).c_str();
	}
};

class WindowException : public MyException
{
public:
	WindowException(const std::string& p_What, int p_Line, const std::string& p_File)
		: MyException(p_What, p_Line, p_File)
	{
	}
};