#pragma once
#include "SpellDefinition.h"

#include <DirectXMath.h>

class SpellInstance
{
public:
	typedef std::shared_ptr<SpellInstance> ptr;

private:
	SpellDefinition::ptr m_SpellDefinition;
	DirectX::XMFLOAT4 m_SpellPosition; //in cm
	DirectX::XMFLOAT4 m_SpellVelocity; //cm per sec
	DirectX::XMFLOAT4 m_SpellColor;
	float m_TimeLived; //in sec

public:
	SpellInstance();
	~SpellInstance();

	void init(SpellDefinition::ptr p_SpellDefinition);
	void update(float p_DeltaTime);

	void explodeSpell(SpellDefinition::ptr p_SpellDefinition);

	void setPosition(DirectX::XMFLOAT4 p_NewPosition);
	void setVelocity(DirectX::XMFLOAT4 p_NewVelocity);
	void setColor(DirectX::XMFLOAT4 p_NewColor);
};

