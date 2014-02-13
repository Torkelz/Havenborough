#pragma once

#include "Actor.h"

#include <map>
#include <memory>

class ActorList : public std::enable_shared_from_this<ActorList>
{
public:
	typedef std::shared_ptr<ActorList> ptr;

private:
	typedef std::map<Actor::Id, Actor::ptr> ActorMap_t;
	ActorMap_t m_Actors;

public:
	void addActor(Actor::ptr p_Actor);
	void removeActor(Actor::Id p_Actor);
	Actor::ptr findActor(Actor::Id p_Actor) const;

	void onUpdate(float p_DeltaTime);

	ActorMap_t::iterator begin();
	ActorMap_t::iterator end();
};