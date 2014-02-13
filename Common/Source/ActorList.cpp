#include "ActorList.h"

#include "CommonExceptions.h"

void ActorList::addActor(Actor::ptr p_Actor)
{
	if (m_Actors.find(p_Actor->getId()) != m_Actors.end())
	{
		throw CommonException("You may not add an already existing actor", __LINE__, __FILE__);
	}

	m_Actors[p_Actor->getId()] = p_Actor;
}

void ActorList::removeActor(Actor::Id p_Actor)
{
	m_Actors.erase(p_Actor);
}

Actor::ptr ActorList::findActor(Actor::Id p_Actor) const
{
	auto actor = m_Actors.find(p_Actor);
	if (actor == m_Actors.end())
	{
		return Actor::ptr();
	}
	else
	{
		return actor->second;
	}
}

void ActorList::onUpdate(float p_DeltaTime)
{
	for (auto& actor : m_Actors)
	{
		actor.second->onUpdate(p_DeltaTime);
	}
}

ActorList::ActorMap_t::iterator ActorList::begin()
{
	return m_Actors.begin();
}

ActorList::ActorMap_t::iterator ActorList::end()
{
	return m_Actors.end();
}
