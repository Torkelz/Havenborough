#pragma once

#include "ActorComponent.h"
#include "EventManager.h"
#include "Utilities/XMFloatUtil.h"
#include "IPhysics.h"

#include <vector>

/**
 * Actor serves as a collection of components that work together to implement
 * the majority of the game logic. Create actors by using an ActorFactory.
 */
class Actor
{
public:
	/**
	 * Shared pointer type.
	 */
	typedef std::shared_ptr<Actor> ptr;
	typedef std::weak_ptr<Actor> wPtr;
	/**
	 * Unique actor identification type.
	 */
	typedef unsigned int Id;

private:
	Id m_Id;
	std::vector<ActorComponent::ptr> m_Components;
	Vector3 m_Position;
	Vector3 m_Rotation;
	EventManager* m_EventManager;

public:
	/**
	 * constructor.
	 *
	 * @param p_Id the unique id to assign to the actor
	 * @param p_EventManager the manager the actor should pass any events to
	 */
	Actor(Id p_Id, EventManager* p_EventManager);
	/**
	 * destructor.
	 */
	~Actor();

	/**
	 * Initialize the actor with any non-component data.
	 *
	 * @param p_Data XML data to read attributes from
	 */
	void initialize(const tinyxml2::XMLElement* p_Data);
	/**
	 * Finish any initialization that must be done after
	 * the actor has been assembled.
	 */
	void postInit();

	/**
	 * Update the actor and all of its components.
	 *
	 * @param p_DeltaTime the time in seconds since last update
	 */
	void onUpdate(float p_DeltaTime);

	/**
	 * Get the id of the actor.
	 *
	 * @return unique actor id
	 */
	Id getId() const;
	/**
	 * Get the position of the actor, which may or may
	 * not be the position of its components.
	 *
	 * @return the current position of the actor in world space (cm)
	 */
	Vector3 getPosition() const;
	/**
	 * Set a new position for the actor.
	 *
	 * @param p_Position a position in world space (cm)
	 */
	void setPosition(Vector3 p_Position);
	/**
	 * Get the rotation of the actor, which may or may
	 * not be the rotation of its components.
	 *
	 * @return the current rotation of the actor as (yaw, pitch, roll) in radians
	 */
	Vector3 getRotation() const;
	/**
	 * Set the rotation of the actor.
	 *
	 * @param a new rotation (yaw, pitch, roll) in radians
	 */
	void setRotation(Vector3 p_Rotation);
	/**
	 * Get the event manager associated with this actor.
	 *
	 * @return the event manager
	 */
	EventManager* getEventManager() const;
	/**
	 * Get all the handles to bodies contained in this actor.
	 *
	 * @return s list of body handles
	 */
	std::vector<BodyHandle> getBodyHandles() const;

	/**
	 * Get the component of the given type, cast to a specific type.
	 *
	 * @param m_Id the unique component type id if the desired component
	 * @return a valid component if one could be found, otherwise an empty pointer.
	 */
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

	void serialize(std::ostream& p_Stream) const;

private:
	friend class ActorFactory;
	void addComponent(ActorComponent::ptr p_Component);
};
