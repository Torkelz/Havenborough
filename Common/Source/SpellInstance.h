#pragma once
#include "SpellDefinition.h"
#include "IPhysics.h"

#include <DirectXMath.h>
#include <vector>

class SpellInstance
{
public:
	typedef std::shared_ptr<SpellInstance> ptr;

private:
	SpellDefinition::ptr m_SpellDefinition;
	IPhysics *m_Physics;
	
	BodyHandle m_Sphere;
	DirectX::XMFLOAT4 m_SpellPosition; //in cm
	DirectX::XMFLOAT4 m_SpellVelocity; //cm per sec
	DirectX::XMFLOAT4 m_SpellColor;
	std::vector<std::pair<unsigned int, DirectX::XMFLOAT4>> bodyHandleDirVList;
	float m_TimeLived; //in sec
	bool m_Collision;

public:
	SpellInstance();
	~SpellInstance();

	void init(SpellDefinition::ptr p_SpellDefinition);
	void update(float p_DeltaTime);

	void spellHit(SpellDefinition::ptr p_SpellDefinition);
	void explodeSpell(SpellDefinition::ptr p_SpellDefinition);
	void changeSphereRadius(float p_NewRadius);

	void setPosition(DirectX::XMFLOAT4 p_NewPosition);
	void setVelocity(DirectX::XMFLOAT4 p_NewVelocity);
	void setColor(DirectX::XMFLOAT4 p_NewColor);
};

