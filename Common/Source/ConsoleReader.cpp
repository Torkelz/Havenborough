#include "ConsoleReader.h"

#include <iostream>

ConsoleReader::ConsoleReader(CommandManager::ptr p_CommandManager)
	: m_CommandManager(p_CommandManager),
	m_ReadThread(&ConsoleReader::read, this)
{
}

ConsoleReader::~ConsoleReader()
{
	m_ReadThread.detach();
}

void ConsoleReader::handleInput()
{
	while (!m_Lines.empty())
	{
		std::string line;
		{
			std::lock_guard<std::mutex> lock(m_LineLock);
			line = m_Lines.front();
			m_Lines.pop();
		}

		if (!line.empty())
		{
			m_CommandManager->runCommand(line);
		}
	}
}

void ConsoleReader::read()
{
	std::string line;
	while (std::getline(std::cin, line))
	{
		std::lock_guard<std::mutex> lock(m_LineLock);
		m_Lines.push(line);
	}
}
