#pragma once

#include "ActorComponent.h"
#include "ClientExceptions.h"

#include <IGraphics.h>
#include <IPhysics.h>

class PhysicsInterface : public ActorComponent
{
public:
	virtual unsigned int getComponentId() const
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
	virtual void updatePosition(Vector3 p_Position) override
	{
		m_Physics->setBodyPosition(m_Body, p_Position);
	}
	virtual void updateRotation(Vector3 p_Rotation) override
	{
		m_Physics->setBodyRotation(m_Body, p_Rotation);
	}
	virtual void onUpdate(float p_DeltaTime) override
	{
		m_Physics->setBodyPosition(m_Body, m_Owner->getPosition());
		Vector3 rotation = m_Owner->getRotation();
		m_Physics->setBodyRotation(m_Body, rotation);
	}
};

class ModelInterface : public ActorComponent
{
public:
	virtual unsigned int getComponentId() const override
	{
		return 2;
	}
	virtual void render() = 0;
};

class ModelComponent : public ModelInterface
{
private:
	int m_Model;
	IGraphics* m_Graphics;

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

		m_Model = m_Graphics->createModelInstance(mesh);
	}
	virtual void render() override
	{
		m_Graphics->renderModel(m_Model);
	}
	virtual void onUpdate(float p_DeltaTime) override
	{
		m_Graphics->setModelPosition(m_Model, m_Owner->getPosition());
		m_Graphics->setModelRotation(m_Model, m_Owner->getRotation());
	}
};