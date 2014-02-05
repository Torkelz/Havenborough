#pragma once
#include "SpellDefinition.h"

#include <DirectXMath.h>

class SpellInstance
{
public:
	typedef std::shared_ptr<SpellInstance> ptr;

private:
	SpellDefinition::ptr m_SpellDefinition;
	DirectX::XMFLOAT4 m_SpellPosition;
	DirectX::XMFLOAT4 m_SpellVelocity;
	DirectX::XMFLOAT4 m_SpellColor;
	

public:
	SpellInstance();
	~SpellInstance();

	void init(SpellDefinition::ptr p_SpellDefinition);
	void update(float p_DeltaTime);

	void setPosition(DirectX::XMFLOAT4 p_NewPosition);
	void setVelocity(DirectX::XMFLOAT4 p_NewVelocity);
	void setColor(DirectX::XMFLOAT4 p_NewColor);
};

