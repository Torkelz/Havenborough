#include "StreamReader.h"

#include <iostream>

#include <vld.h>

StreamReader::StreamReader(CommandManager::ptr p_CommandManager, std::istream& p_InputStream)
	: m_CommandManager(p_CommandManager),
	m_InputStream(p_InputStream)
{
	VLDDisable();
	m_ReadThread = boost::thread(&StreamReader::read, this);
	VLDEnable();
}

StreamReader::~StreamReader()
{
	if (m_InputStream == std::cin)
	{
		m_ReadThread.detach();
	}
	else if (m_ReadThread.joinable())
	{
		m_ReadThread.join();
	}
}

void StreamReader::handleInput()
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

void StreamReader::readAll()
{
	if (m_ReadThread.joinable())
	{
		m_ReadThread.join();
	}
}

void StreamReader::read()
{
	if (m_InputStream == std::cin)
		VLDDisable();

	std::string line;
	while (std::getline(m_InputStream, line))
	{
		std::lock_guard<std::mutex> lock(m_LineLock);
		m_Lines.push(line);
	}
}
