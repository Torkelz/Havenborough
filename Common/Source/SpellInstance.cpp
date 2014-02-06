#include "SpellInstance.h"


SpellInstance::SpellInstance()
{
	m_SpellPosition = DirectX::XMFLOAT4(0.f, 0.f, 0.f, 0.f);
	m_SpellVelocity = DirectX::XMFLOAT4(0.f, 0.f, 0.f, 0.f);
	m_SpellColor =  DirectX::XMFLOAT4(0.f, 0.f, 0.f, 0.f);
	m_TimeLived = 0.f;
	
}


SpellInstance::~SpellInstance()
{
}

void SpellInstance::init(SpellDefinition::ptr p_SpellDefinition)
{
	m_SpellDefinition = p_SpellDefinition;
}

void SpellInstance::update(float p_DeltaTime)
{
	if(m_TimeLived > m_SpellDefinition->maxTimeToLive)
	{
		m_TimeLived += p_DeltaTime;
	}
	else
	{
		explodeSpell(m_SpellDefinition);
	}
}

void SpellInstance::explodeSpell(SpellDefinition::ptr p_SpellDefinition)
{

}

void SpellInstance::setPosition(DirectX::XMFLOAT4 p_NewPosition)
{
	m_SpellPosition = p_NewPosition;
}

void SpellInstance::setVelocity(DirectX::XMFLOAT4 p_NewVelocity)
{
	m_SpellVelocity = p_NewVelocity;
}

void SpellInstance::setColor(DirectX::XMFLOAT4 p_NewColor)
{
	m_SpellColor = p_NewColor;
}