#pragma once

#include <memory>

class ActorFactory
{
public:
	typedef std::shared_ptr<ActorFactory> ptr;

private:
	uint16_t m_PrevActorId;

public:
	ActorFactory();

	uint16_t getNextActorId();
};
