#pragma once
#include "ActorComponent.h"
#include "EventData.h"
#include "ResourceManager.h"
#include "CommonExceptions.h"
#include "XMLHelper.h"

#include <IPhysics.h>


/**
 * Interface for a physics component.
 * 
 * Use for things like collisions and forces.
 */
class PhysicsInterface : public ActorComponent
{
public:
	static const Id m_ComponentId = 1;	/// Unique id
	Id getComponentId() const override
	{
		return m_ComponentId;
	}

	/**
	 * Get the body handle of the component.
	 *
	 * @return a body handle
	 */
	virtual BodyHandle getBodyHandle() const = 0;

	virtual Vector3 getVelocity() const = 0;

	virtual bool isInAir() const = 0;
};

/**
 * Oriented bounding box component.
 */
class OBB_Component : public PhysicsInterface
{
private:
	BodyHandle m_Body;
	IPhysics* m_Physics;
	Vector3 m_OffsetPositition;
	bool m_Immovable;
	Vector3 m_Halfsize;

public:
	~OBB_Component() override
	{
		m_Physics->releaseBody(m_Body);
	}

	/**
	 * Set the physics to use for the component.
	 *
	 * @param p_Physics the physics library to use
	 */
	void setPhysics(IPhysics* p_Physics)
	{
		m_Physics = p_Physics;
	}

	void initialize(const tinyxml2::XMLElement* p_Data) override
	{
		m_OffsetPositition = Vector3(0.f, 0.f, 0.f);

		m_Halfsize = Vector3(1.f, 1.f, 1.f);
		const tinyxml2::XMLElement* size = p_Data->FirstChildElement("Halfsize");
		if (size)
		{
			m_Halfsize.x = size->FloatAttribute("x");
			m_Halfsize.y = size->FloatAttribute("y");
			m_Halfsize.z = size->FloatAttribute("z");
		}

		const tinyxml2::XMLElement* relPos = p_Data->FirstChildElement("OffsetPosition");
		if (relPos)
		{
			relPos->QueryAttribute("x", &m_OffsetPositition.x);
			relPos->QueryAttribute("y", &m_OffsetPositition.y);
			relPos->QueryAttribute("z", &m_OffsetPositition.z);
		}

		m_Immovable = true;
		p_Data->QueryBoolAttribute("Immovable", &m_Immovable);
	}

	void postInit() override
	{
		m_Body = m_Physics->createOBB(0.f, m_Immovable, m_Owner->getPosition() + m_OffsetPositition, m_Halfsize, false);
		m_Physics->setBodyRotation(m_Body, m_Owner->getRotation());
	}

	void serialize(tinyxml2::XMLPrinter& p_Printer) const override
	{
		p_Printer.OpenElement("OBBPhysics");
		p_Printer.PushAttribute("Immovable", m_Immovable);
		pushVector(p_Printer, "Halfsize", m_Halfsize);
		pushVector(p_Printer, "OffsetPosition", m_OffsetPositition);
		p_Printer.CloseElement();
	}

	void onUpdate(float p_DeltaTime) override
	{
		m_Owner->setPosition(m_Physics->getBodyPosition(m_Body) - m_OffsetPositition);
		Vector3 rotation = m_Owner->getRotation();
		m_Physics->setBodyRotation(m_Body, rotation);
	}

	void setPosition(Vector3 p_Position) override
	{
		m_Physics->setBodyPosition(m_Body, p_Position + m_OffsetPositition);
	}

	void setRotation(Vector3 p_Rotation) override
	{
		m_Physics->setBodyRotation(m_Body, p_Rotation);
	}

	BodyHandle getBodyHandle() const override
	{
		return m_Body;
	}

	Vector3 getVelocity() const override
	{
		return m_Physics->getBodyVelocity(m_Body);
	}

	bool isInAir() const override
	{
		return m_Physics->getBodyInAir(m_Body);
	}
};

/**
 * Bounding sphere component.
 */
class CollisionSphereComponent : public PhysicsInterface
{
private:
	BodyHandle m_Body;
	IPhysics* m_Physics;
	Vector3 m_OffsetPositition;
	float m_Radius;
	float m_Mass;
	bool m_Immovable;

public:
	~CollisionSphereComponent() override
	{
		m_Physics->releaseBody(m_Body);
	}
	
	/**
	 * Set the physics to use for the component.
	 *
	 * @param p_Physics the physics library to use
	 */
	void setPhysics(IPhysics* p_Physics)
	{
		m_Physics = p_Physics;
	}

	void initialize(const tinyxml2::XMLElement* p_Data) override
	{
		m_OffsetPositition = Vector3(0.f, 0.f, 0.f);

		const tinyxml2::XMLElement* relPos = p_Data->FirstChildElement("OffsetPosition");
		if (relPos)
		{
			relPos->QueryAttribute("x", &m_OffsetPositition.x);
			relPos->QueryAttribute("y", &m_OffsetPositition.y);
			relPos->QueryAttribute("z", &m_OffsetPositition.z);
		}

		m_Immovable = true;
		p_Data->QueryBoolAttribute("Immovable", &m_Immovable);

		m_Radius = 1.f;
		p_Data->QueryFloatAttribute("Radius", &m_Radius);

		m_Mass = 0.f;
		p_Data->QueryAttribute("Mass", &m_Mass);
	}

	void postInit() override
	{
		m_Body = m_Physics->createSphere(m_Mass, m_Immovable, m_Owner->getPosition() + m_OffsetPositition, m_Radius);
	}

	void serialize(tinyxml2::XMLPrinter& p_Printer) const override
	{
		p_Printer.OpenElement("SpherePhysics");
		p_Printer.PushAttribute("Immovable", m_Immovable);
		p_Printer.PushAttribute("Radius", m_Radius);
		p_Printer.PushAttribute("Mass", m_Mass);
		pushVector(p_Printer, "OffsetPosition", m_OffsetPositition);
		p_Printer.CloseElement();
	}

	void onUpdate(float p_DeltaTime) override
	{
		m_Owner->setPosition(m_Physics->getBodyPosition(m_Body) - m_OffsetPositition);
		Vector3 rotation = m_Owner->getRotation();
		m_Physics->setBodyRotation(m_Body, rotation);
	}

	void setPosition(Vector3 p_Position) override
	{
		m_Physics->setBodyPosition(m_Body, p_Position + m_OffsetPositition);
	}

	BodyHandle getBodyHandle() const override
	{
		return m_Body;
	}

	Vector3 getVelocity() const override
	{
		return m_Physics->getBodyVelocity(m_Body);
	}

	bool isInAir() const override
	{
		return m_Physics->getBodyInAir(m_Body);
	}
};

/**
 * Axis-Aligned Bounding Box component.
 */
class AABB_Component : public PhysicsInterface
{
private:
	BodyHandle m_Body;
	IPhysics* m_Physics;
	Vector3 m_OffsetPositition;
	Vector3 m_Halfsize;
	bool m_IsEdge;
	bool m_RespondToCollision;

public:
	~AABB_Component() override
	{
		m_Physics->releaseBody(m_Body);
	}
	
	/**
	 * Set the physics to use for the component.
	 *
	 * @param p_Physics the physics library to use
	 */
	void setPhysics(IPhysics* p_Physics)
	{
		m_Physics = p_Physics;
	}

	void initialize(const tinyxml2::XMLElement* p_Data) override
	{
		m_OffsetPositition = Vector3(0.f, 0.f, 0.f);

		m_Halfsize = Vector3(1.f, 1.f, 1.f);
		const tinyxml2::XMLElement* size = p_Data->FirstChildElement("Halfsize");
		if (size)
		{
			m_Halfsize.x = size->FloatAttribute("x");
			m_Halfsize.y = size->FloatAttribute("y");
			m_Halfsize.z = size->FloatAttribute("z");
		}

		const tinyxml2::XMLElement* relPos = p_Data->FirstChildElement("OffsetPosition");
		if (relPos)
		{
			relPos->QueryAttribute("x", &m_OffsetPositition.x);
			relPos->QueryAttribute("y", &m_OffsetPositition.y);
			relPos->QueryAttribute("z", &m_OffsetPositition.z);
		}

		m_IsEdge = false;
		p_Data->QueryBoolAttribute("Edge", &m_IsEdge);
		m_RespondToCollision = true;
		p_Data->QueryBoolAttribute("CollisionResponse", &m_RespondToCollision);
	}

	void postInit() override
	{
		m_Body = m_Physics->createAABB(0.f, true, m_Owner->getPosition() + m_OffsetPositition, m_Halfsize, m_IsEdge);
		m_Physics->setBodyCollisionResponse(m_Body, m_RespondToCollision);
	}

	void serialize(tinyxml2::XMLPrinter& p_Printer) const override
	{
		p_Printer.OpenElement("AABBPhysics");
		p_Printer.PushAttribute("Edge", m_IsEdge);
		p_Printer.PushAttribute("CollisionResponse", m_RespondToCollision);
		pushVector(p_Printer, "Halfsize", m_Halfsize);
		pushVector(p_Printer, "OffsetPosition", m_OffsetPositition);
		p_Printer.CloseElement();
	}

	void onUpdate(float p_DeltaTime) override
	{
		m_Owner->setPosition(m_Physics->getBodyPosition(m_Body) - m_OffsetPositition);
	}

	void setPosition(Vector3 p_Position) override
	{
		m_Physics->setBodyPosition(m_Body, p_Position + m_OffsetPositition);
	}

	BodyHandle getBodyHandle() const override
	{
		return m_Body;
	}

	Vector3 getVelocity() const override
	{
		return m_Physics->getBodyVelocity(m_Body);
	}

	bool isInAir() const override
	{
		return m_Physics->getBodyInAir(m_Body);
	}
};

/**
 * Bounding volume component based on a triangle mesh.
 */
class BoundingMeshComponent : public PhysicsInterface
{
private:
	BodyHandle m_Body;
	int m_MeshResourceId;
	IPhysics* m_Physics;
	ResourceManager* m_ResourceManager;
	Vector3 m_Scale;
	std::string m_MeshName;

public:
	~BoundingMeshComponent() override
	{
		m_Physics->releaseBody(m_Body);
		m_ResourceManager->releaseResource(m_MeshResourceId);
	}
	
	/**
	 * Set the physics to use for the component.
	 *
	 * @param p_Physics the physics library to use
	 */
	void setPhysics(IPhysics* p_Physics)
	{
		m_Physics = p_Physics;
	}
	/**
	 * Set the resource manager for the component.
	 *
	 * @param p_ResourceManager the resource manager to use
	 */
	void setResourceManager(ResourceManager* p_ResourceManager)
	{
		m_ResourceManager = p_ResourceManager;
	}

	void initialize(const tinyxml2::XMLElement* p_Data) override
	{
		const char* meshName = p_Data->Attribute("Mesh");

		if (!meshName)
		{
			throw CommonException("Collision component lacks mesh", __LINE__, __FILE__);
		}

		m_MeshName = meshName;
		m_MeshResourceId = m_ResourceManager->loadResource("volume", meshName);

		m_Scale = Vector3(1.f, 1.f, 1.f);
		const tinyxml2::XMLElement* scale = p_Data->FirstChildElement("Scale");
		if (scale)
		{
			m_Scale.x = scale->FloatAttribute("x");
			m_Scale.y = scale->FloatAttribute("y");
			m_Scale.z = scale->FloatAttribute("z");
		}
	}

	void postInit() override
	{
		m_Body = m_Physics->createBVInstance(m_MeshName.c_str());
		m_Physics->setBodyPosition(m_Body, m_Owner->getPosition());
		m_Physics->setBodyRotation(m_Body, m_Owner->getRotation());
		m_Physics->setBodyScale(m_Body, m_Scale);
	}

	void serialize(tinyxml2::XMLPrinter& p_Printer) const override
	{
		p_Printer.OpenElement("MeshPhysics");
		p_Printer.PushAttribute("Mesh", m_MeshName.c_str());
		pushVector(p_Printer, "Scale", m_Scale);
		p_Printer.CloseElement();
	}

	void setPosition(Vector3 p_Position) override
	{
		m_Physics->setBodyPosition(m_Body, p_Position);
	}

	void setRotation(Vector3 p_Rotation) override
	{
		m_Physics->setBodyRotation(m_Body, p_Rotation);
	}

	BodyHandle getBodyHandle() const override
	{
		return m_Body;
	}

	Vector3 getVelocity() const override
	{
		return m_Physics->getBodyVelocity(m_Body);
	}

	bool isInAir() const override
	{
		return m_Physics->getBodyInAir(m_Body);
	}
};

/**
 * Interface for model components.
 */
class ModelInterface : public ActorComponent
{
public:
	static const Id m_ComponentId = 2;	/// Unique id
	Id getComponentId() const override
	{
		return m_ComponentId;
	}
	/**
	 * Update the scale of the model.
	 *
	 * @param p_CompName an identifier to keep track of the scale to keep track of it
	 * @param p_Scale the new scale
	 */
	virtual void updateScale(const std::string& p_CompName, Vector3 p_Scale) = 0;
	/**
	 * Remove a scale from the model.
	 *
	 * @param p_CompName an identifier of an existing scale
	 */
	virtual void removeScale(const std::string& p_CompName) = 0;
	/**
	 * Change a color tone for the model.
	 *
	 * @param p_ColorTone the color in RGB range 0.0f to 1.0f
	 */
	virtual void setColorTone(const Vector3 p_ColorTone) = 0;
};

/**
 * A standard model component.
 */
class ModelComponent : public ModelInterface
{
public:
	/**
	 * Id for separating different model components.
	 */
	typedef unsigned int ModelCompId;

private:
	ModelCompId m_Id;
	Vector3 m_BaseScale;
	Vector3 m_ColorTone;
	std::string m_MeshName;
	std::vector<std::pair<std::string, Vector3>> m_AppliedScales;

public:
	~ModelComponent() override
	{
		m_Owner->getEventManager()->queueEvent(IEventData::Ptr(new RemoveMeshEventData(m_Id)));
	}

	void initialize(const tinyxml2::XMLElement* p_Data) override
	{
		const char* mesh = p_Data->Attribute("Mesh");
		if (!mesh)
		{
			throw CommonException("Component lacks mesh", __LINE__, __FILE__);
		}

		m_MeshName = std::string(mesh);

		m_BaseScale = Vector3(1.f, 1.f, 1.f);
		const tinyxml2::XMLElement* scale = p_Data->FirstChildElement("Scale");
		if (scale)
		{
			scale->QueryFloatAttribute("x", &m_BaseScale.x);
			scale->QueryFloatAttribute("y", &m_BaseScale.y);
			scale->QueryFloatAttribute("z", &m_BaseScale.z);
		}

		m_ColorTone = Vector3(1.f, 1.f, 1.f);
		const tinyxml2::XMLElement* tone = p_Data->FirstChildElement("ColorTone");
		if (tone)
		{
			tone->QueryFloatAttribute("x", &m_ColorTone.x);
			tone->QueryFloatAttribute("y", &m_ColorTone.y);
			tone->QueryFloatAttribute("z", &m_ColorTone.z);
		}
	}
	void postInit() override
	{
		m_Owner->getEventManager()->queueEvent(IEventData::Ptr(new CreateMeshEventData(m_Id, m_MeshName,
			m_BaseScale, m_ColorTone)));
		setPosition(m_Owner->getPosition());
		setRotation(m_Owner->getRotation());
	}

	void serialize(tinyxml2::XMLPrinter& p_Printer) const override
	{
		p_Printer.OpenElement("Model");
		p_Printer.PushAttribute("Mesh", m_MeshName.c_str());
		pushVector(p_Printer, "Scale", m_BaseScale);
		pushVector(p_Printer, "ColorTone", m_ColorTone);
		p_Printer.CloseElement();
	}

	void setPosition(Vector3 p_Position) override
	{
		m_Owner->getEventManager()->queueEvent(IEventData::Ptr(new UpdateModelPositionEventData(m_Id, p_Position)));
	}

	void setRotation(Vector3 p_Rotation) override
	{
		m_Owner->getEventManager()->queueEvent(IEventData::Ptr(new UpdateModelRotationEventData(m_Id, p_Rotation)));
	}

	void updateScale(const std::string& p_CompName, Vector3 p_Scale) override
	{
		for (auto& scale : m_AppliedScales)
		{
			if (scale.first == p_CompName)
			{
				scale.second = p_Scale;
				calculateScale();
				return;
			}
		}

		m_AppliedScales.push_back(std::make_pair(p_CompName, p_Scale));

		calculateScale();
	}
	void removeScale(const std::string& p_CompName) override
	{
		for (auto& scale : m_AppliedScales)
		{
			if (scale.first == p_CompName)
			{
				std::swap(scale, m_AppliedScales.back());
				m_AppliedScales.pop_back();
				return;
			}
		}
	}

	void setColorTone(const Vector3 p_ColorTone) override
	{
		m_ColorTone = p_ColorTone;

		m_Owner->getEventManager()->queueEvent(IEventData::Ptr(new ChangeColorToneEvent(m_Id, m_ColorTone)));
	}

	/**
	 * Get the model component id from the component.
	 *
	 * @return unique id of the model
	 */
	ModelCompId getId() const
	{
		return m_Id;
	}

	/**
	 * Set the id of the model component.
	 *
	 * @param p_Id a new unique identifier
	 */
	void setId(ModelCompId p_Id)
	{
		m_Id = p_Id;
	}

private:
	void calculateScale()
	{
		Vector3 composedScale = m_BaseScale;
		for (const auto& scale : m_AppliedScales)
		{
			composedScale.x *= scale.second.x;
			composedScale.y *= scale.second.y;
			composedScale.z *= scale.second.z;
		}
		m_Owner->getEventManager()->queueEvent(IEventData::Ptr(new UpdateModelScaleEventData(getId(), composedScale)));
	}

};

/**
 * Interface for movements.
 */
class MovementInterface : public ActorComponent
{
public:
	static const Id m_ComponentId = 3;	/// Unique id
	Id getComponentId() const override
	{
		return m_ComponentId;
	}

	/**
	 * Set the velocity of the movement.
	 *
	 * @param p_Velocity velocity in cm/s
	 */
	virtual void setVelocity(Vector3 p_Velocity) = 0;
	/**
	 * Get the velocity.
	 *
	 * @return the current velocity of the movement in cm/s
	 */
	virtual Vector3 getVelocity() const = 0;
	/**
	 * Set the velocity of rotation.
	 *
	 * @param p_RotVelocity the (yaw, pitch, roll) rotation velocity in radians/s
	 */
	virtual void setRotationalVelocity(Vector3 p_RotVelocity) = 0;
	/**
	 * Get the current rotational velocity.
	 *
	 * @return the (yaw, pitch, roll) rotation velocity in radians/s
	 */
	virtual Vector3 getRotationalVelocity() const = 0;
};

/**
 * Simple linear movement component implementation.
 */
class MovementComponent : public MovementInterface
{
private:
	Vector3 m_Velocity;
	Vector3 m_RotVelocity;

public:
	void initialize(const tinyxml2::XMLElement* p_Data) override
	{
		const tinyxml2::XMLElement* velElem = p_Data->FirstChildElement("Velocity");
		if (velElem)
		{
			m_Velocity.x = velElem->FloatAttribute("x");
			m_Velocity.y = velElem->FloatAttribute("y");
			m_Velocity.z = velElem->FloatAttribute("z");
		}
		else
		{
			m_Velocity = Vector3(0.f, 0.f, 0.f);
		}

		const tinyxml2::XMLElement* rotVelElem = p_Data->FirstChildElement("RotationalVelocity");
		if (rotVelElem)
		{
			m_RotVelocity.x = rotVelElem->FloatAttribute("x");
			m_RotVelocity.y = rotVelElem->FloatAttribute("y");
			m_RotVelocity.z = rotVelElem->FloatAttribute("z");
		}
		else
		{
			m_RotVelocity = Vector3(0.f, 0.f, 0.f);
		}
	}

	void onUpdate(float p_DeltaTime) override
	{
		const Vector3 pos = m_Owner->getPosition();
		m_Owner->setPosition(pos + m_Velocity * p_DeltaTime);

		const Vector3 rot = m_Owner->getRotation();
		m_Owner->setRotation(rot + m_RotVelocity * p_DeltaTime);
	}

	void serialize(tinyxml2::XMLPrinter& p_Printer) const override
	{
		p_Printer.OpenElement("Movement");
		pushVector(p_Printer, "Velocity", m_Velocity);
		pushVector(p_Printer, "RotationalVelocity", m_RotVelocity);
		p_Printer.CloseElement();
	}

	void setVelocity(Vector3 p_Velocity) override
	{
		m_Velocity = p_Velocity;
	}
	Vector3 getVelocity() const override
	{
		return m_Velocity;
	}
	void setRotationalVelocity(Vector3 p_RotVelocity) override
	{
		m_RotVelocity = p_RotVelocity;
	}
	Vector3 getRotationalVelocity() const override
	{
		return m_RotVelocity;
	}
};

/**
 * Simple linear movement component implementation.
 */
class CircleMovementComponent : public MovementInterface
{
private:
	Vector3 m_CircleCenterPosition;
	float m_CircleAngle;
	float m_CircleRotationSpeed;
	float m_CircleRadius;

public:
	void initialize(const tinyxml2::XMLElement* p_Data) override
	{
		m_CircleCenterPosition = Vector3(0.f, 0.f, 0.f);
		m_CircleAngle = 0.f;
		m_CircleRotationSpeed = PI;
		m_CircleRadius = 100.f;

		const tinyxml2::XMLElement* centerElem = p_Data->FirstChildElement("CircleCenter");
		if (centerElem)
		{
			m_CircleCenterPosition.x = centerElem->FloatAttribute("x");
			m_CircleCenterPosition.y = centerElem->FloatAttribute("y");
			m_CircleCenterPosition.z = centerElem->FloatAttribute("z");
		}

		p_Data->QueryFloatAttribute("StartAngle", &m_CircleAngle);
		p_Data->QueryFloatAttribute("RotationSpeed", &m_CircleRotationSpeed);
		p_Data->QueryFloatAttribute("CircleRadius", &m_CircleRadius);
	}

	void postInit() override
	{
		onUpdate(0.f);
	}

	void onUpdate(float p_DeltaTime) override
	{
		m_CircleAngle += m_CircleRotationSpeed * p_DeltaTime;
		Vector3 newPos = m_CircleCenterPosition;
		newPos.x += cos(m_CircleAngle) * m_CircleRadius;
		newPos.z += -sin(m_CircleAngle) * m_CircleRadius;
		Vector3 newRot(m_CircleAngle, 0.f, m_CircleAngle);

		m_Owner->setPosition(newPos);
		m_Owner->setRotation(newRot);
	}

	void serialize(tinyxml2::XMLPrinter& p_Printer) const override
	{
		p_Printer.OpenElement("CircleMovement");
		p_Printer.PushAttribute("StartAngle", m_CircleAngle);
		p_Printer.PushAttribute("RotationSpeed", m_CircleRotationSpeed);
		p_Printer.PushAttribute("CircleRadius", m_CircleRadius);
		pushVector(p_Printer, "CircleCenter", m_CircleCenterPosition);
		p_Printer.CloseElement();
	}

	void setVelocity(Vector3 p_Velocity) override
	{
	}
	Vector3 getVelocity() const override
	{
		Vector3 newVel;
		newVel.x = -sin(m_CircleAngle) * m_CircleRadius * m_CircleRotationSpeed;
		newVel.y = 0.f;
		newVel.z = -cos(m_CircleAngle) * m_CircleRadius * m_CircleRotationSpeed;
		return newVel;
	}
	void setRotationalVelocity(Vector3 p_RotVelocity) override
	{
	}
	Vector3 getRotationalVelocity() const override
	{
		return Vector3(m_CircleRotationSpeed, 0.f, m_CircleRotationSpeed);
	}
	Vector3 getCenterPosition() const
	{
		return m_CircleCenterPosition;
	}
	float getRadius() const
	{
		return m_CircleRadius;
	}
};

/**
 * Interface for visibly pulsing an actor.
 */
class PulseInterface : public ActorComponent
{
public:
	static const Id m_ComponentId = 4;	/// Unique id
	virtual Id getComponentId() const override
	{
		return m_ComponentId;
	}
	/**
	 * Activate a single pulse.
	 */
	virtual void pulseOnce() = 0;
};

/**
 * Scaling component implementation of pulsing.
 */
class PulseComponent : public PulseInterface
{
private:
	float m_PulseLength;
	float m_PulseStrength;
	float m_CurrentTime;

public:
	void initialize(const tinyxml2::XMLElement* p_Data) override
	{
		m_PulseLength = 1.f;
		p_Data->QueryFloatAttribute("Length", &m_PulseLength);
		
		m_PulseStrength = 0.5f;
		p_Data->QueryAttribute("Strength", &m_PulseLength);

		m_CurrentTime = m_PulseLength;
	}

	void onUpdate(float p_DeltaTime) override
	{
		if (m_CurrentTime < m_PulseLength)
		{
			m_CurrentTime += p_DeltaTime;
			if (m_CurrentTime < m_PulseLength)
			{
				static const float pi = 3.141592f;
				Vector3 appliedScale = Vector3(1.f, 1.f, 1.f) * (1.f + sin(pi * m_CurrentTime / m_PulseLength) * m_PulseStrength);

				std::shared_ptr<ModelInterface> modelComp(m_Owner->getComponent<ModelInterface>(2));
				if (modelComp)
				{
					modelComp->updateScale("PulseComp", appliedScale);
				}
			}
			else
			{
				std::shared_ptr<ModelInterface> modelComp(m_Owner->getComponent<ModelInterface>(2));
				if (modelComp)
				{
					modelComp->removeScale("PulseComp");
				}
			}
		}
	}

	void serialize(tinyxml2::XMLPrinter& p_Printer) const override
	{
		p_Printer.OpenElement("Pulse");
		p_Printer.PushAttribute("Length", m_PulseLength);
		p_Printer.PushAttribute("Strength", m_PulseStrength);
		p_Printer.CloseElement();
	}

	void pulseOnce() override
	{
		m_CurrentTime = 0.f;
	}
};

/**
 * Interface for components shining light.
 */
class LightInterface : public ActorComponent
{
public:
	static const Id m_ComponentId = 5;	/// Unique id
	Id getComponentId() const override
	{
		return m_ComponentId;
	}
};

/**
 * Light component implementation providing point, spot and directional lights.
 */
class LightComponent : public LightInterface
{
private:
	Light m_Light;

public:
	~LightComponent() override
	{
		m_Owner->getEventManager()->queueEvent(IEventData::Ptr(new RemoveLightEventData(m_Light.id)));
	}

	void initialize(const tinyxml2::XMLElement* p_Data) override
	{
		if (p_Data->Attribute("Type", "Point"))
		{
			Vector3 position(0.f, 0.f, 0.f);
			Vector3 color(1.f, 1.f, 1.f);
			float range = 2000.f;

			p_Data->QueryAttribute("Range", &range);

			const tinyxml2::XMLElement* pos = p_Data->FirstChildElement("Position");
			if (pos)
			{
				pos->QueryAttribute("x", &position.x);
				pos->QueryAttribute("y", &position.y);
				pos->QueryAttribute("z", &position.z);
			}

			const tinyxml2::XMLElement* col = p_Data->FirstChildElement("Color");
			if (col)
			{
				col->QueryAttribute("r", &color.x);
				col->QueryAttribute("g", &color.y);
				col->QueryAttribute("b", &color.z);
			}

			m_Light = Light::createPointLight(position, range, color);
		}
		else if (p_Data->Attribute("Type", "Spot"))
		{
			Vector3 position(0.f, 0.f, 0.f);
			Vector3 direction(0.f, -1.f, 0.f);
			Vector3 color(1.f, 1.f, 1.f);
			Vector2 angles(1.f, 1.f);
			float range = 2000.f;

			p_Data->QueryAttribute("Range", &range);

			const tinyxml2::XMLElement* pos = p_Data->FirstChildElement("Position");
			if (pos)
			{
				pos->QueryAttribute("x", &position.x);
				pos->QueryAttribute("y", &position.y);
				pos->QueryAttribute("z", &position.z);
			}

			const tinyxml2::XMLElement* dir = p_Data->FirstChildElement("Direction");
			if (dir)
			{
				dir->QueryAttribute("x", &direction.x);
				dir->QueryAttribute("y", &direction.y);
				dir->QueryAttribute("z", &direction.z);
			}

			const tinyxml2::XMLElement* col = p_Data->FirstChildElement("Color");
			if (col)
			{
				col->QueryAttribute("r", &color.x);
				col->QueryAttribute("g", &color.y);
				col->QueryAttribute("b", &color.z);
			}

			const tinyxml2::XMLElement* ang = p_Data->FirstChildElement("Angles");
			if (ang)
			{
				ang->QueryAttribute("min", &angles.x);
				ang->QueryAttribute("max", &angles.y);
			}

			m_Light = Light::createSpotLight(position, direction, angles, range, color);
		}
		else if (p_Data->Attribute("Type", "Directional"))
		{
			Vector3 direction(0.f, -1.f, 0.f);
			Vector3 color(1.f, 1.f, 1.f);

			const tinyxml2::XMLElement* dir = p_Data->FirstChildElement("Direction");
			if (dir)
			{
				dir->QueryAttribute("x", &direction.x);
				dir->QueryAttribute("y", &direction.y);
				dir->QueryAttribute("z", &direction.z);
			}

			const tinyxml2::XMLElement* col = p_Data->FirstChildElement("Color");
			if (col)
			{
				col->QueryAttribute("r", &color.x);
				col->QueryAttribute("g", &color.y);
				col->QueryAttribute("b", &color.z);
			}

			m_Light = Light::createDirectionalLight(direction, color);
		}
		else
		{
			throw CommonException("XML Light description missing valid type", __LINE__, __FILE__);
		}
	}
	void postInit() override
	{
		m_Owner->getEventManager()->queueEvent(IEventData::Ptr(new LightEventData(m_Light)));
	}

	void serialize(tinyxml2::XMLPrinter& p_Printer) const override
	{
		p_Printer.OpenElement("Light");
		switch (m_Light.type)
		{
		case Light::Type::DIRECTIONAL:
			{
				p_Printer.PushAttribute("Type", "Directional");
				pushVector(p_Printer, "Direction", m_Light.direction);
			}
			break;

		case Light::Type::POINT:
			{
				p_Printer.PushAttribute("Type", "Point");
				p_Printer.PushAttribute("Range", m_Light.range);
				pushVector(p_Printer, "Position", m_Light.position);
			}
			break;

		case Light::Type::SPOT:
			{
				p_Printer.PushAttribute("Type", "Spot");
				p_Printer.PushAttribute("Range", m_Light.range);
				pushVector(p_Printer, "Position", m_Light.position);
				pushVector(p_Printer, "Direction", m_Light.direction);
				p_Printer.OpenElement("Angles");
				p_Printer.PushAttribute("min", m_Light.spotlightAngles.x);
				p_Printer.PushAttribute("max", m_Light.spotlightAngles.y);
				p_Printer.CloseElement();
			}
			break;

		default:
			p_Printer.PushAttribute("Type", "Unknown");
			break;
		};
		pushColor(p_Printer, "Color", m_Light.color);
		p_Printer.CloseElement();
	}

	/**
	 * Get the unique id of the light.
	 *
	 * @return the lights unique identifier
	 */
	Light::Id getId() const
	{
		return m_Light.id;
	}

	/**
	 * Set a new unique identifier for the light.
	 *
	 * @param p_Id the light's id
	 */
	void setId(Light::Id p_Id)
	{
		m_Light.id = p_Id;
	}
};

class LookInterface : public ActorComponent
{
public:
	static const Id m_ComponentId = 6;	/// Unique id
	virtual Id getComponentId() const override
	{
		return m_ComponentId;
	}
	virtual Vector3 getLookPosition() const = 0;
	virtual Vector3 getLookForward() const = 0;
	virtual void setLookForward(Vector3 p_Forward) = 0;
	virtual Vector3 getLookUp() const = 0;
	virtual void setLookUp(Vector3 p_Up) = 0;
	virtual Vector3 getLookRight() const = 0;
	virtual DirectX::XMFLOAT4X4 getRotationMatrix() const = 0;
};

class LookComponent : public LookInterface
{
private:
	Vector3 m_OffsetPosition;
	Vector3 m_Forward;
	Vector3 m_Up;

public:
	void initialize(const tinyxml2::XMLElement* p_Data) override
	{
		m_OffsetPosition = Vector3(0.f, 0.f, 0.f);
		m_Forward = Vector3(0.f, 0.f, 1.f);
		m_Up = Vector3(0.f, 1.f, 0.f);

		queryVector(p_Data->FirstChildElement("OffsetPosition"), m_OffsetPosition);
		queryVector(p_Data->FirstChildElement("Forward"), m_Forward);
		queryVector(p_Data->FirstChildElement("Up"), m_Up);
	}

	void serialize(tinyxml2::XMLPrinter& p_Printer) const override
	{
		p_Printer.OpenElement("Look");
		pushVector(p_Printer, "OffsetPosition", m_OffsetPosition);
		pushVector(p_Printer, "Forward", m_Forward);
		pushVector(p_Printer, "Up", m_Up);
		p_Printer.CloseElement();
	}

	Vector3 getLookPosition() const override
	{
		return m_Owner->getPosition() + m_OffsetPosition;
	}

	Vector3 getLookForward() const override
	{
		return m_Forward;
	}

	void setLookForward(Vector3 p_Forward) override
	{
		m_Forward = p_Forward;
	}

	Vector3 getLookUp() const override
	{
		return m_Up;
	}

	void setLookUp(Vector3 p_Up) override
	{
		m_Up = p_Up;
	}

	Vector3 getLookRight() const override
	{
		DirectX::XMVECTOR rightV = DirectX::XMVector3Cross(
			DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(m_Up)),
			DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(m_Forward)));
		Vector3 right;
		DirectX::XMStoreFloat3((DirectX::XMFLOAT3*)&right, rightV);

		return right;
	}

	DirectX::XMFLOAT4X4 getRotationMatrix() const override
	{
		Vector3 right = getLookRight();

		return DirectX::XMFLOAT4X4(
			right.x, right.y, right.z, 0.f,
			m_Up.x, m_Up.y, m_Up.z, 0.f,
			m_Forward.x, m_Forward.y, m_Forward.z, 0.f,
			0.f, 0.f, 0.f, 1.f);
	}
};

class ParticleInterface : public ActorComponent
{
public:
	static const Id m_ComponentId = 7;	/// Unique id
	virtual Id getComponentId() const override
	{
		return m_ComponentId;
	}
};

class ParticleComponent : public ParticleInterface
{
private:
	unsigned int m_ParticleId;
	std::string m_EffectName;

public:
	~ParticleComponent()
	{
		m_Owner->getEventManager()->queueEvent(IEventData::Ptr(new RemoveParticleEventData(m_ParticleId)));
	}

	void initialize(const tinyxml2::XMLElement* p_Data) override
	{
		const char* effectName = p_Data->Attribute("Effect");
		if (!effectName)
		{
			throw CommonException("Missing effect name", __LINE__, __FILE__);
		}

		m_EffectName = effectName;
	}

	void postInit() override
	{
		m_Owner->getEventManager()->queueEvent(IEventData::Ptr(new CreateParticleEventData(m_ParticleId, m_EffectName, m_Owner->getPosition())));
	}

	void serialize(tinyxml2::XMLPrinter& p_Printer) const override
	{
		p_Printer.OpenElement("Particle");
		p_Printer.PushAttribute("Effect", m_EffectName.c_str());
		p_Printer.CloseElement();
	}

	/**
	 * Get the unique id of the particle effect.
	 *
	 * @return the particle effect's unique identifier
	 */
	unsigned int getId() const
	{
		return m_ParticleId;
	}

	/**
	 * Set a new unique identifier for the particle effect.
	 *
	 * @param p_Id the particle effect's id
	 */
	void setId(unsigned int p_Id)
	{
		m_ParticleId = p_Id;
	}
};

class AnimationInterface : public ActorComponent
{
public:
	static const Id m_ComponentId = 8;	/// Unique id
	virtual Id getComponentId() const override
	{
		return m_ComponentId;
	}
};
