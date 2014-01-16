#pragma once

#include "Actor.h"

#include <IGraphics.h>
#include <IPhysics.h>

#include <tinyxml2.h>

#include <functional>
#include <map>
#include <string>

class ActorFactory
{
private:
	unsigned int m_LastActorId;
	unsigned int m_LastModelComponentId;
	IGraphics* m_Graphics;
	IPhysics* m_Physics;
	EventManager* m_EventManager;

protected:
	typedef std::function<ActorComponent::ptr()> componentCreatorFunc;
	std::map<std::string, componentCreatorFunc> m_ComponentCreators;

public:
	ActorFactory();

	void setGraphics(IGraphics* p_Graphics);
	void setPhysics(IPhysics* p_Physics);
	void setEventManager(EventManager* p_EventManager);

	Actor::ptr createActor(const tinyxml2::XMLElement* p_Data);
	Actor::ptr createActor(const tinyxml2::XMLElement* p_Data, Actor::Id p_Id);

protected:
	virtual ActorComponent::ptr createComponent(const tinyxml2::XMLElement* p_Data);

private:
	unsigned int getNextActorId();

	ActorComponent::ptr createOBBComponent();
	ActorComponent::ptr createAABBComponent();
	ActorComponent::ptr createModelComponent();
	ActorComponent::ptr createMovementComponent();
	ActorComponent::ptr createPulseComponent();
};
