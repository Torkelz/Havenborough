#include "LookComponent.h"

#include "HumanAnimationComponent.h"

void LookComponent::initialize(const tinyxml2::XMLElement* p_Data)
{
	m_OffsetPosition = Vector3(0.f, 0.f, 0.f);
	m_Forward = Vector3(0.f, 0.f, 1.f);
	m_Up = Vector3(0.f, 1.f, 0.f);

	queryVector(p_Data->FirstChildElement("OffsetPosition"), m_OffsetPosition);
	queryVector(p_Data->FirstChildElement("Forward"), m_Forward);
	queryVector(p_Data->FirstChildElement("Up"), m_Up);
}

void LookComponent::serialize(tinyxml2::XMLPrinter& p_Printer) const
{
	p_Printer.OpenElement("Look");
	pushVector(p_Printer, "OffsetPosition", m_OffsetPosition);
	pushVector(p_Printer, "Forward", m_Forward);
	pushVector(p_Printer, "Up", m_Up);
	p_Printer.CloseElement();
}

Vector3 LookComponent::getLookPosition() const
{
	std::shared_ptr<HumanAnimationComponent> comp = m_Owner->getComponent<HumanAnimationComponent>(HumanAnimationComponent::m_ComponentId).lock();
	if (comp)
	{
		return comp->getJointPos("Head");
	}
	else
	{
		return m_Owner->getPosition() + m_OffsetPosition;
	}
}

Vector3 LookComponent::getLookForward() const
{
	return m_Forward;
}

void LookComponent::setLookForward(Vector3 p_Forward)
{
	m_Forward = p_Forward;
}

Vector3 LookComponent::getLookUp() const
{
	return m_Up;
}

void LookComponent::setLookUp(Vector3 p_Up)
{
	m_Up = p_Up;
}

Vector3 LookComponent::getLookRight() const
{
	DirectX::XMVECTOR rightV = DirectX::XMVector3Cross(
		DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(m_Up)),
		DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(m_Forward)));
	Vector3 right;
	DirectX::XMStoreFloat3((DirectX::XMFLOAT3*)&right, rightV);

	return right;
}

DirectX::XMFLOAT4X4 LookComponent::getRotationMatrix() const
{
	Vector3 right = getLookRight();

	return DirectX::XMFLOAT4X4(
		right.x, right.y, right.z, 0.f,
		m_Up.x, m_Up.y, m_Up.z, 0.f,
		m_Forward.x, m_Forward.y, m_Forward.z, 0.f,
		0.f, 0.f, 0.f, 1.f);
}
