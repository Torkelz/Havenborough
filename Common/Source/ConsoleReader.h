#pragma once

#include "CommandManager.h"

#include <mutex>
#include <queue>
#include <thread>

class ConsoleReader
{
public:
	typedef std::shared_ptr<ConsoleReader> ptr;

private:
	CommandManager::ptr m_CommandManager;

	std::queue<std::string> m_Lines;
	std::thread m_ReadThread;
	std::mutex m_LineLock;

public:
	explicit ConsoleReader(CommandManager::ptr p_CommandManager);
	~ConsoleReader();

	void handleInput();

private:
	void read();
};
