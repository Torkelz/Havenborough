#include "ActorFactory.h"

ActorFactory::ActorFactory()
	:	m_PrevActorId(0)
{
}

uint16_t ActorFactory::getNextActorId()
{
	return ++m_PrevActorId;
}
