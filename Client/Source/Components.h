#pragma once

#include "ActorComponent.h"
#include "ClientExceptions.h"

#include <IGraphics.h>
#include <IPhysics.h>

class PhysicsInterface : public ActorComponent
{
public:
	virtual Id getComponentId() const
	{
		return 1;
	}

	virtual void updatePosition(Vector3 p_Position) = 0;
	virtual void updateRotation(Vector3 p_Rotation) = 0;
};

class OBB_Component : public PhysicsInterface
{
private:
	BodyHandle m_Body;
	IPhysics* m_Physics;

public:
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

		m_Body = m_Physics->createOBB(0.f, true, position, halfsize, false);
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
};

class ModelInterface : public ActorComponent
{
public:
	virtual Id getComponentId() const override
	{
		return 2;
	}
	virtual void render() = 0;
	virtual void updateScale(const std::string& p_CompName, Vector3 p_Scale) = 0;
	virtual void removeScale(const std::string& p_CompName) = 0;
};

class ModelComponent : public ModelInterface
{
private:
	int m_Model;
	IGraphics* m_Graphics;
	Vector3 m_Scale;
	std::vector<std::pair<std::string, Vector3>> m_AppliedScales;

public:
	void setGraphics(IGraphics* p_Graphics)
	{
		m_Graphics = p_Graphics;
	}

	virtual void initialize(const tinyxml2::XMLElement* p_Data) override
	{
		const char* mesh = p_Data->Attribute("Mesh");
		if (!mesh)
		{
			throw ClientException("Component lacks mesh", __LINE__, __FILE__);
		}

		m_Scale = Vector3(1.f, 1.f, 1.f);
		const tinyxml2::XMLElement* scale = p_Data->FirstChildElement("Scale");
		if (scale)
		{
			scale->QueryFloatAttribute("x", &m_Scale.x);
			scale->QueryFloatAttribute("y", &m_Scale.y);
			scale->QueryFloatAttribute("z", &m_Scale.z);
		}

		m_Model = m_Graphics->createModelInstance(mesh);
	}
	virtual void render() override
	{
		m_Graphics->renderModel(m_Model);
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
		m_Graphics->setModelPosition(m_Model, m_Owner->getPosition());
		m_Graphics->setModelRotation(m_Model, m_Owner->getRotation());

		Vector3 composedScale = m_Scale;
		for (const auto& scale : m_AppliedScales)
		{
			composedScale.x *= scale.second.x;
			composedScale.y *= scale.second.y;
			composedScale.z *= scale.second.z;
		}
		m_Graphics->setModelScale(m_Model, composedScale);
	}
};

class MovementInterface : public ActorComponent
{
public:
	virtual Id getComponentId() const override
	{
		return 3;
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
	virtual Id getComponentId() const override
	{
		return 4;
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
