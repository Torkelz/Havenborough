/**
 *
 */
#pragma once

#include "CommandManager.h"

#include <mutex>
#include <queue>
#include <thread>

/**
 * A command-line reader for the default console. Avoid.
 */
class ConsoleReader
{
public:
	/**
	 * Smart pointer type.
	 */
	typedef std::shared_ptr<ConsoleReader> ptr;

private:
	CommandManager::ptr m_CommandManager;

	std::queue<std::string> m_Lines;
	std::thread m_ReadThread;
	std::mutex m_LineLock;

public:
	/**
	 * constructor.
	 *
	 * @param p_CommandManager the command manager where read commands should be sent.
	 */
	explicit ConsoleReader(CommandManager::ptr p_CommandManager);
	/**
	 * destructor.
	 */
	~ConsoleReader();

	/**
	 * Handle any received input.
	 *
	 * In order to prevent data races, the ConsoleReader queue up
	 * received commands until handleInput is called by the main thread.
	 */
	void handleInput();

private:
	void read();
};
