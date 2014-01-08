#pragma once

#include <tinyxml2.h>

#include <memory>

class Actor;

class ActorComponent
{
protected:
	Actor* m_Owner;

public:
	typedef std::shared_ptr<ActorComponent> ptr;

	virtual ~ActorComponent() {}

	virtual void initialize(const tinyxml2::XMLElement* p_Data) = 0;
	virtual void postInit() {}
	virtual void onUpdate(float p_DeltaTime) {}

	virtual unsigned int getComponentId() const = 0;

private:
	friend class ActorFactory;

	void setOwner(Actor* p_Owner) { m_Owner = p_Owner; }
};
