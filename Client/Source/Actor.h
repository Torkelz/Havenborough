#pragma once

#include "ActorComponent.h"
#include "../Utilities/XMFloatUtil.h"

#include <tinyxml2.h>

#include <vector>

class Actor
{
public:
	typedef std::shared_ptr<Actor> ptr;
	typedef unsigned int Id;

private:
	Id m_Id;
	std::vector<ActorComponent::ptr> m_Components;
	Vector3 m_Position;
	Vector3 m_Rotation;

public:
	explicit Actor(Id p_Id);
	~Actor();

	void initialize(const tinyxml2::XMLElement* p_Data);
	void postInit();

	void onUpdate(float p_DeltaTime);

	Id getId() const;
	Vector3 getPosition() const;
	void setPosition(Vector3 p_Position);
	Vector3 getRotation() const;
	void setRotation(Vector3 p_Rotation);

	template <class ComponentType>
	std::weak_ptr<ComponentType> getComponent(unsigned int m_Id)
	{
		for (auto& comp : m_Components)
		{
			if (comp->getComponentId() == m_Id)
			{
				std::shared_ptr<ComponentType> sub(std::static_pointer_cast<ComponentType>(comp));
				return std::weak_ptr<ComponentType>(sub);
			}
		}

		return std::weak_ptr<ComponentType>();
	}

private:
	friend class ActorFactory;
	void addComponent(ActorComponent::ptr p_Component);
};
