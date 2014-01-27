/**
 * Stuff.
 */

#pragma once

#include <tinyxml2/tinyxml2.h>

#include <memory>

class Actor;

/**
 * Base class for actor components.
 */
class ActorComponent
{
protected:
	Actor* m_Owner;

public:
	/**
	 * Shared pointer type.
	 */
	typedef std::shared_ptr<ActorComponent> ptr;
	/**
	 * Unique component identifier type to separate different types of components.
	 */
	typedef unsigned int Id;

	/**
	 * destructor.
	 */
	virtual ~ActorComponent() {}

	/**
	 * Initialize the component from XML data.
	 *
	 * @param p_Data the data containing parameters to read for initializing.
	 */
	virtual void initialize(const tinyxml2::XMLElement* p_Data) = 0;
	/**
	 * Perform any finishing initialization that requires
	 * that the component has been added to an actor,
	 * along with any other components.
	 */
	virtual void postInit() {}
	/**
	 * Perform any update logic.
	 *
	 * @param p_DeltaTime the time in seconds since previous update.
	 */
	virtual void onUpdate(float p_DeltaTime) {}

	/**
	 * Get the components type id.
	 *
	 * @return the id of the component type
	 */
	virtual Id getComponentId() const = 0;

private:
	friend class ActorFactory;

	void setOwner(Actor* p_Owner) { m_Owner = p_Owner; }
};
