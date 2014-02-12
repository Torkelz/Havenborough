#pragma once

#include "Actor.h"
#include "AnimationLoader.h"
#include "ResourceManager.h"
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
public:
	typedef std::shared_ptr<ActorFactory> ptr;

private:
	unsigned int m_LastActorId;
	unsigned int m_LastModelComponentId;
	unsigned int m_LastLightComponentId;
	unsigned int m_LastParticleComponentId;
	IPhysics* m_Physics;
	EventManager* m_EventManager;
	ResourceManager* m_ResourceManager;
	AnimationLoader* m_AnimationLoader;

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
	explicit ActorFactory(unsigned int p_BaseActorId);

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

	void setAnimationLoader(AnimationLoader* p_AnimationLoader);

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

	// ************ Test methods ************
	Actor::ptr createRotatingBox(Vector3 p_Position, Vector3 p_Scale);
	Actor::ptr createSkybox(Vector3 p_Scale);
	Actor::ptr createBasicModel(const std::string& p_Model, Vector3 p_Position);
	Actor::ptr createCollisionSphere(Vector3 p_Position, float p_Radius);
	Actor::ptr createCheckPointActor(Vector3 p_Position, Vector3 p_Scale, Vector3 p_ColorTone);
	std::string getPlayerActorDescription(Vector3 p_Position) const;
	Actor::ptr createPlayerActor(Vector3 p_Position);
	Actor::ptr createDirectionalLight(Vector3 p_Direction, Vector3 p_Color);
	Actor::ptr createSpotLight(Vector3 p_Position, Vector3 p_Direction, Vector2 p_MinMaxAngles, float p_Range, Vector3 p_Color);
	Actor::ptr createPointLight(Vector3 p_Position, float p_Range, Vector3 p_Color);
	Actor::ptr createCheckPointActor(Vector3 p_Position, Vector3 p_Scale);
	Actor::ptr createCheckPointArrow();
	Actor::ptr createParticles(Vector3 p_Position, const std::string& p_Effect);
	Actor::ptr createBoxWithOBB(Vector3 p_Position, Vector3 p_Halfsize, Vector3 p_Rotation);
	struct InstanceModel
	{
		Vector3 position;
		Vector3 rotation;
		Vector3 scale;
		std::string meshName;
	};
	struct InstanceBoundingVolume
	{
		Vector3 scale;
		std::string meshName;
	};
	struct InstanceEdgeBox
	{
		Vector3 offsetPosition;
		Vector3 offsetRotation;
		Vector3 halfsize;
	};
	Actor::ptr createInstanceActor(
		const InstanceModel& p_Model,
		const std::vector<InstanceBoundingVolume>& p_BoundingVolumes,
		const std::vector<InstanceEdgeBox>& p_Edges);

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
	ActorComponent::ptr createCircleMovementComponent();
	ActorComponent::ptr createPulseComponent();
	ActorComponent::ptr createLightComponent();
	ActorComponent::ptr createParticleComponent();
	ActorComponent::ptr createLookComponent();
	ActorComponent::ptr createHumanAnimationComponent();

	void print(tinyxml2::XMLPrinter& p_Printer, const InstanceModel& p_Model);
	void print(tinyxml2::XMLPrinter& p_Printer, const InstanceBoundingVolume& p_Volume);
	void print(tinyxml2::XMLPrinter& p_Printer, const InstanceEdgeBox& p_Edge);
};
