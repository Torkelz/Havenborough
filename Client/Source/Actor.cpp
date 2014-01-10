#include "Actor.h"

Actor::Actor(Id p_Id)
	: m_Id(p_Id)
{
}

Actor::~Actor()
{
}

void Actor::initialize(const tinyxml2::XMLElement* p_Data)
{

}

void Actor::postInit()
{
}

void Actor::onUpdate(float p_DeltaTime)
{
	for (auto& comp : m_Components)
	{
		comp->onUpdate(p_DeltaTime);
	}
}

Actor::Id Actor::getId() const
{
	return m_Id;
}

Vector3 Actor::getPosition() const
{
	return m_Position;
}

void Actor::setPosition(Vector3 p_Position)
{
	m_Position = p_Position;
}

Vector3 Actor::getRotation() const
{
	return m_Rotation;
}

void Actor::setRotation(Vector3 p_Rotation)
{
	m_Rotation = p_Rotation;
}

void Actor::addComponent(ActorComponent::ptr p_Component)
{
	m_Components.push_back(p_Component);
}
