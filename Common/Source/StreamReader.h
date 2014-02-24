/**
 *
 */
#pragma once

#include "CommandManager.h"

#include <boost/thread.hpp>

#include <mutex>
#include <queue>

/**
 * A command-line reader for any stream.
 *
 * If possible, avoid using it with the default console.
 */
class StreamReader
{
public:
	/**
	 * Smart pointer type.
	 */
	typedef std::shared_ptr<StreamReader> ptr;

private:
	CommandManager::ptr m_CommandManager;
	std::istream& m_InputStream;

	std::queue<std::string> m_Lines;
	boost::thread m_ReadThread;
	std::mutex m_LineLock;

public:
	/**
	 * constructor.
	 *
	 * @param p_CommandManager the command manager where read commands should be sent.
	 * @param p_InputStream the stream where commands should be read from.
	 *			The stream should not be read from any other thread while this object exists,
	 *			and the caller is responsible for making sure the lifetime of the stream outlasts this object.
	 */
	StreamReader(CommandManager::ptr p_CommandManager, std::istream& p_InputStream);
	/**
	 * destructor.
	 */
	~StreamReader();

	/**
	 * Handle any received input.
	 *
	 * In order to prevent data races, the StreamReader queue up
	 * received commands until handleInput is called by the main thread.
	 */
	void handleInput();

	/**
	 * Blocks until all of the stream has been read.
	 *
	 * This should not be done on a stream that would block when there is no data.
	 */
	void readAll();

private:
	void read();
};
