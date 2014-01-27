/**
 * Stuff.
 */

#pragma once

#include "Actor.h"
#include "ResourceManager.h"

#include <IGraphics.h>
#include <IPhysics.h>

#include <tinyxml2/tinyxml2.h>

#include <functional>
#include <map>
#include <string>

/**
 * Factory for producing actors, fully filled with components.
 */
class ActorFactory
{
private:
	unsigned int m_LastActorId;
	unsigned int m_LastModelComponentId;
	unsigned int m_LastLightComponentId;
	IGraphics* m_Graphics;
	IPhysics* m_Physics;
	EventManager* m_EventManager;
	ResourceManager* m_ResourceManager;

protected:
	/**
	 * Component creation function type.
	 */
	typedef std::function<ActorComponent::ptr()> componentCreatorFunc;
	/**
	 * Mapping of creator function to different component names.
	 */
	std::map<std::string, componentCreatorFunc> m_ComponentCreators;

public:
	/**
	 * constructor.
	 */
	ActorFactory();

	/**
	 * Set the graphics library to be used when creating components requiring graphics.
	 *
	 * @param p_Graphics the graphics library to use
	 */
	void setGraphics(IGraphics* p_Graphics);
	/**
	 * Set the physics library to be used when creating component requiring physics.
	 *
	 * @param p_Physics the physics library to use
	 */
	void setPhysics(IPhysics* p_Physics);
	/**
	 * Set the event manager actors created with this factory will use.
	 *
	 * @param p_EventManager the event manager to use
	 */
	void setEventManager(EventManager* p_EventManager);
	/**
	 * Set the resource manager actor created with this factory will use for resource management.
	 *
	 * @param p_ResourceManager the resource manager to use
	 */
	void setResourceManager(ResourceManager* p_ResourceManager);

	/**
	 * Create an actor from a XML description, with a unique id.
	 *
	 * @param p_Data XML actor description
	 */
	Actor::ptr createActor(const tinyxml2::XMLElement* p_Data);
	/**
	 * Create an actor from a XML description, using the given id.
	 *
	 * @param p_Data XML actor description
	 * @param p_Id the id to give to the actor, should be unique.
	 */
	Actor::ptr createActor(const tinyxml2::XMLElement* p_Data, Actor::Id p_Id);

protected:
	/**
	 * Creata a component from a XML description.
	 *
	 * @param p_Data the XML description to create a component from
	 */
	virtual ActorComponent::ptr createComponent(const tinyxml2::XMLElement* p_Data);

private:
	unsigned int getNextActorId();

	ActorComponent::ptr createOBBComponent();
	ActorComponent::ptr createAABBComponent();
	ActorComponent::ptr createCollisionSphereComponent();
	ActorComponent::ptr createBoundingMeshComponent();
	ActorComponent::ptr createModelComponent();
	ActorComponent::ptr createMovementComponent();
	ActorComponent::ptr createPulseComponent();
	ActorComponent::ptr createLightComponent();
};
