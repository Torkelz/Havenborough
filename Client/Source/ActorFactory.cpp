#include "ActorFactory.h"

#include "ClientExceptions.h"
#include "Components.h"

ActorFactory::ActorFactory()
	:	m_LastActorId(65536),
		m_Graphics(nullptr),
		m_Physics(nullptr)
{
	m_ComponentCreators["OBBPhysics"] = std::bind(&ActorFactory::createOBBComponent, this);
	m_ComponentCreators["Model"] = std::bind(&ActorFactory::createModelComponent, this);
	m_ComponentCreators["Movement"] = std::bind(&ActorFactory::createMovementComponent, this);
}

void ActorFactory::setGraphics(IGraphics* p_Graphics)
{
	m_Graphics = p_Graphics;
}

void ActorFactory::setPhysics(IPhysics* p_Physics)
{
	m_Physics = p_Physics;
}

Actor::ptr ActorFactory::createActor(const tinyxml2::XMLElement* p_Data)
{
	return createActor(p_Data, getNextActorId());
}

Actor::ptr ActorFactory::createActor(const tinyxml2::XMLElement* p_Data, Actor::Id p_Id)
{
	Actor::ptr actor(new Actor(p_Id));
	actor->initialize(p_Data);

	for (const tinyxml2::XMLElement* node = p_Data->FirstChildElement(); node; node = node->NextSiblingElement())
	{
		ActorComponent::ptr component(createComponent(node));
		if (component)
		{
			actor->addComponent(component);
			component->setOwner(actor.get());
		}
		else
		{
			return Actor::ptr();
		}
	}

	actor->postInit();

	return actor;
}

ActorComponent::ptr ActorFactory::createComponent(const tinyxml2::XMLElement* p_Data)
{
	std::string name(p_Data->Value());

	ActorComponent::ptr component;

	auto findIt = m_ComponentCreators.find(name);
	if (findIt != m_ComponentCreators.end())
	{
		componentCreatorFunc creator = findIt->second;
		component = creator();
	}
	else
	{
		throw ClientException("Could not find ActorComponent creator named '" + name + "'", __LINE__, __FILE__);
	}

	if (component)
	{
		component->initialize(p_Data);
	}

	return component;
}

unsigned int ActorFactory::getNextActorId()
{
	return ++m_LastActorId;
}

ActorComponent::ptr ActorFactory::createOBBComponent()
{
	OBB_Component* comp = new OBB_Component;
	comp->setPhysics(m_Physics);

	return ActorComponent::ptr(comp);
}

ActorComponent::ptr ActorFactory::createModelComponent()
{
	ModelComponent* comp = new ModelComponent;
	comp->setGraphics(m_Graphics);

	return ActorComponent::ptr(comp);
}

ActorComponent::ptr ActorFactory::createMovementComponent()
{
	return  ActorComponent::ptr(new MovementComponent);
}
