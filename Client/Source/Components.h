#pragma once

#include "ActorComponent.h"
#include "ClientExceptions.h"
#include "EventData.h"
#include "ResourceManager.h"

#include <IGraphics.h>
#include <IPhysics.h>

class PhysicsInterface : public ActorComponent
{
public:
	static const Id m_ComponentId = 1;
	virtual Id getComponentId() const
	{
		return m_ComponentId;
	}

	virtual void updatePosition(Vector3 p_Position) = 0;
	virtual void updateRotation(Vector3 p_Rotation) = 0;
	virtual BodyHandle getBodyHandle() const = 0;
};

class OBB_Component : public PhysicsInterface
{
private:
	BodyHandle m_Body;
	IPhysics* m_Physics;

public:
	virtual ~OBB_Component()
	{
		m_Physics->releaseBody(m_Body);
	}

	void setPhysics(IPhysics* p_Physics)
	{
		m_Physics = p_Physics;
	}

	virtual void initialize(const tinyxml2::XMLElement* p_Data) override
	{
		Vector3 position(0.f, 0.f, 0.f);
		const tinyxml2::XMLElement* pos = p_Data->FirstChildElement("Position");
		if (pos)
		{
			position.x = pos->FloatAttribute("x");
			position.y = pos->FloatAttribute("y");
			position.z = pos->FloatAttribute("z");
		}

		Vector3 halfsize(1.f, 1.f, 1.f);
		const tinyxml2::XMLElement* size = p_Data->FirstChildElement("Halfsize");
		if (size)
		{
			halfsize.x = size->FloatAttribute("x");
			halfsize.y = size->FloatAttribute("y");
			halfsize.z = size->FloatAttribute("z");
		}

		bool immovable = true;
		p_Data->QueryBoolAttribute("Immovable", &immovable);

		m_Body = m_Physics->createOBB(0.f, immovable, position, halfsize, false);
	}

	virtual void onUpdate(float p_DeltaTime) override
	{
		m_Physics->setBodyPosition(m_Body, m_Owner->getPosition());
		Vector3 rotation = m_Owner->getRotation();
		m_Physics->setBodyRotation(m_Body, rotation);
	}

	virtual void updatePosition(Vector3 p_Position) override
	{
		m_Physics->setBodyPosition(m_Body, p_Position);
	}
	virtual void updateRotation(Vector3 p_Rotation) override
	{
		m_Physics->setBodyRotation(m_Body, p_Rotation);
	}
	virtual BodyHandle getBodyHandle() const override
	{
		return m_Body;
	}
};

class CollisionSphereComponent : public PhysicsInterface
{
private:
	BodyHandle m_Body;
	IPhysics* m_Physics;

public:
	virtual ~CollisionSphereComponent()
	{
		m_Physics->releaseBody(m_Body);
	}

	void setPhysics(IPhysics* p_Physics)
	{
		m_Physics = p_Physics;
	}

	virtual void initialize(const tinyxml2::XMLElement* p_Data) override
	{
		Vector3 position(0.f, 0.f, 0.f);
		const tinyxml2::XMLElement* pos = p_Data->FirstChildElement("Position");
		if (pos)
		{
			position.x = pos->FloatAttribute("x");
			position.y = pos->FloatAttribute("y");
			position.z = pos->FloatAttribute("z");
		}

		bool immovable = true;
		p_Data->QueryBoolAttribute("Immovable", &immovable);

		float radius = 1.f;
		p_Data->QueryFloatAttribute("Radius", &radius);

		m_Body = m_Physics->createSphere(0.f, immovable, position, radius);
	}

	virtual void onUpdate(float p_DeltaTime) override
	{
		m_Physics->setBodyPosition(m_Body, m_Owner->getPosition());
		Vector3 rotation = m_Owner->getRotation();
		m_Physics->setBodyRotation(m_Body, rotation);
	}

	virtual void updatePosition(Vector3 p_Position) override
	{
		m_Physics->setBodyPosition(m_Body, p_Position);
	}
	virtual void updateRotation(Vector3 p_Rotation) override
	{
		m_Physics->setBodyRotation(m_Body, p_Rotation);
	}
	virtual BodyHandle getBodyHandle() const override
	{
		return m_Body;
	}
};

class AABB_Component : public PhysicsInterface
{
private:
	BodyHandle m_Body;
	IPhysics* m_Physics;
	Vector3 m_RelativePosition;

public:
	void setPhysics(IPhysics* p_Physics)
	{
		m_Physics = p_Physics;
	}

	virtual void initialize(const tinyxml2::XMLElement* p_Data) override
	{
		m_RelativePosition = Vector3(0.f, 0.f, 0.f);
		const tinyxml2::XMLElement* pos = p_Data->FirstChildElement("RelativePosition");
		if (pos)
		{
			m_RelativePosition.x = pos->FloatAttribute("x");
			m_RelativePosition.y = pos->FloatAttribute("y");
			m_RelativePosition.z = pos->FloatAttribute("z");
		}

		Vector3 halfsize(1.f, 1.f, 1.f);
		const tinyxml2::XMLElement* size = p_Data->FirstChildElement("Halfsize");
		if (size)
		{
			halfsize.x = size->FloatAttribute("x");
			halfsize.y = size->FloatAttribute("y");
			halfsize.z = size->FloatAttribute("z");
		}

		bool edge = false;
		p_Data->QueryBoolAttribute("Edge", &edge);

		m_Body = m_Physics->createAABB(0.f, true, Vector3(0.f, 0.f, 0.f), halfsize, edge);
	}

	virtual void onUpdate(float p_DeltaTime) override
	{
		m_Physics->setBodyPosition(m_Body, m_Owner->getPosition() + m_RelativePosition);
	}

	virtual void updatePosition(Vector3 p_Position) override
	{
		m_Physics->setBodyPosition(m_Body, p_Position + m_RelativePosition);
	}
	virtual void updateRotation(Vector3 p_Rotation) override
	{
		m_Physics->setBodyRotation(m_Body, p_Rotation);
	}
	virtual BodyHandle getBodyHandle() const override
	{
		return m_Body;
	}
};

class BoundingMeshComponent : public PhysicsInterface
{
private:
	BodyHandle m_Body;
	int m_MeshResourceId;
	IPhysics* m_Physics;
	ResourceManager* m_ResourceManager;
	Vector3 m_Scale;

public:
	~BoundingMeshComponent()
	{
		m_ResourceManager->releaseResource(m_MeshResourceId);
	}

	void setPhysics(IPhysics* p_Physics)
	{
		m_Physics = p_Physics;
	}
	void setResourceManager(ResourceManager* p_ResourceManager)
	{
		m_ResourceManager = p_ResourceManager;
	}

	virtual void initialize(const tinyxml2::XMLElement* p_Data) override
	{
		const char* meshName = p_Data->Attribute("Mesh");

		if (!meshName)
		{
			throw ClientException("Collision component lacks mesh", __LINE__, __FILE__);
		}

		m_MeshResourceId = m_ResourceManager->loadResource("volume", meshName);

		m_Scale = Vector3(1.f, 1.f, 1.f);
		const tinyxml2::XMLElement* scale = p_Data->FirstChildElement("Scale");
		if (scale)
		{
			m_Scale.x = scale->FloatAttribute("x");
			m_Scale.y = scale->FloatAttribute("y");
			m_Scale.z = scale->FloatAttribute("z");
		}

		m_Body = m_Physics->createBVInstance(meshName);
		m_Physics->setBodyScale(m_Body, m_Scale);
	}

	virtual void updatePosition(Vector3 p_Position) override
	{
		m_Physics->setBodyPosition(m_Body, p_Position);
	}
	virtual void updateRotation(Vector3 p_Rotation) override
	{
		m_Physics->setBodyRotation(m_Body, p_Rotation);
	}
	virtual BodyHandle getBodyHandle() const override
	{
		return m_Body;
	}
};

class ModelInterface : public ActorComponent
{
public:
	static const Id m_ComponentId = 2;
	virtual Id getComponentId() const override
	{
		return m_ComponentId;
	}
	//virtual void render(IGraphics* p_Graphics) = 0;
	virtual void updateScale(const std::string& p_CompName, Vector3 p_Scale) = 0;
	virtual void removeScale(const std::string& p_CompName) = 0;
};

class ModelComponent : public ModelInterface
{
public:
	typedef unsigned int Id;

private:
	Id m_Id;
	Vector3 m_BaseScale;
	std::string m_MeshName;
	std::vector<std::pair<std::string, Vector3>> m_AppliedScales;

public:
	virtual void initialize(const tinyxml2::XMLElement* p_Data) override
	{
		const char* mesh = p_Data->Attribute("Mesh");
		if (!mesh)
		{
			throw ClientException("Component lacks mesh", __LINE__, __FILE__);
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
	}
	virtual void postInit() override
	{
		m_Owner->getEventManager()->queueEvent(IEventData::Ptr(new CreateMeshEventData(m_Id, m_MeshName, m_BaseScale)));
	}
	virtual void updateScale(const std::string& p_CompName, Vector3 p_Scale) override
	{
		for (auto& scale : m_AppliedScales)
		{
			if (scale.first == p_CompName)
			{
				scale.second = p_Scale;
				return;
			}
		}

		m_AppliedScales.push_back(std::make_pair(p_CompName, p_Scale));
	}
	virtual void removeScale(const std::string& p_CompName) override
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
	virtual void onUpdate(float p_DeltaTime) override
	{
		//if (m_Graphics)
		//{
		//	m_Graphics->setModelPosition(m_Model, m_Owner->getPosition());
		//	m_Graphics->setModelRotation(m_Model, m_Owner->getRotation());

		//	Vector3 composedScale = m_BaseScale;
		//	for (const auto& scale : m_AppliedScales)
		//	{
		//		composedScale.x *= scale.second.x;
		//		composedScale.y *= scale.second.y;
		//		composedScale.z *= scale.second.z;
		//	}
		//	m_Graphics->setModelScale(m_Model, composedScale);
		//}
	}

	Id getId() const
	{
		return m_Id;
	}

	void setId(Id p_Id)
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
		m_Owner->getEventManager()->queueEvent(IEventData::Ptr(new UpdateModelRotationEventData(getId(), composedScale)));
	}

};

class MovementInterface : public ActorComponent
{
public:
	static const Id m_ComponentId = 3;
	virtual Id getComponentId() const override
	{
		return m_ComponentId;
	}
	virtual void setVelocity(Vector3 p_Velocity) = 0;
	virtual Vector3 getVelocity() const = 0;
	virtual void setRotationalVelocity(Vector3 p_RotVelocity) = 0;
	virtual Vector3 getRotationalVelocity() const = 0;
};

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

class PulseInterface : public ActorComponent
{
public:
	static const Id m_ComponentId = 4;
	virtual Id getComponentId() const override
	{
		return m_ComponentId;
	}
	virtual void pulseOnce() = 0;
};

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

	void pulseOnce()
	{
		m_CurrentTime = 0.f;
	}
};
