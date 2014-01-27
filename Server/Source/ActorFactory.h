/**
 * Stuff.
 */

#pragma once

#include <memory>

/**
 * Factory for creating server actors.
 */
class ActorFactory
{
public:
	/**
	 * Shared pointer type.
	 */
	typedef std::shared_ptr<ActorFactory> ptr;

private:
	uint16_t m_PrevActorId;

public:
	/**
	 * contructor.
	 */
	ActorFactory();

	/**
	 * Get the next unique actor id.
	 * <p>
	 * The ids will eventually wrap around, so no more than 65535 actors please!
	 *
	 * @return a unique actor id.
	 */
	uint16_t getNextActorId();
};
