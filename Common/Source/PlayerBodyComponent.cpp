#include "PlayerBodyComponent.h"
#include "RunControlComponent.h"

PlayerBodyComponent::~PlayerBodyComponent()
{
	m_Physics->releaseBody(m_Body);
}

void PlayerBodyComponent::setPhysics(IPhysics* p_Physics)
{
	m_Physics = p_Physics;
}

void PlayerBodyComponent::initialize(const tinyxml2::XMLElement* p_Data)
{
	m_RadiusMain = 1.f;
	p_Data->QueryFloatAttribute("RadiusMain", &m_RadiusMain);
	m_RadiusAnkle = 1.f;
	p_Data->QueryFloatAttribute("RadiusAnkle", &m_RadiusAnkle);
	m_RadiusHead = 1.f;
	p_Data->QueryFloatAttribute("RadiusHead", &m_RadiusHead);
	m_Mass = 1.f;
	p_Data->QueryFloatAttribute("Mass", &m_Mass);
	m_FallTime = 0.f;
	p_Data->QueryFloatAttribute("FallTime", &m_FallTime);
	m_FallTolerance = 0.f;
	p_Data->QueryFloatAttribute("FallTolerance", &m_FallTolerance);

	m_Scale = Vector3(1.f, 1.f, 1.f);
	const tinyxml2::XMLElement* scale = p_Data->FirstChildElement("Scale");
	if (scale)
	{
		m_Scale.x = scale->FloatAttribute("x");
		m_Scale.y = scale->FloatAttribute("y");
		m_Scale.z = scale->FloatAttribute("z");
	}

	m_HalfsizeBox = Vector3(1.f, 1.f, 1.f);
	const tinyxml2::XMLElement* sizeBox = p_Data->FirstChildElement("HalfsizeBox");
	if (sizeBox)
	{
		m_HalfsizeBox.x = sizeBox->FloatAttribute("x");
		m_HalfsizeBox.y = sizeBox->FloatAttribute("y");
		m_HalfsizeBox.z = sizeBox->FloatAttribute("z");
	}

	m_OffsetPositionSphereHead = Vector3(0.f, 0.f, 0.f);
	const tinyxml2::XMLElement* relPosSphereHead = p_Data->FirstChildElement("OffsetPositionSphereHead");
	if (relPosSphereHead)
	{
		relPosSphereHead->QueryAttribute("x", &m_OffsetPositionSphereHead.x);
		relPosSphereHead->QueryAttribute("y", &m_OffsetPositionSphereHead.y);
		relPosSphereHead->QueryAttribute("z", &m_OffsetPositionSphereHead.z);
	}

	m_OffsetPositionSphereMain = Vector3(0.f, 0.f, 0.f);
	const tinyxml2::XMLElement* relPosSphereMain = p_Data->FirstChildElement("OffsetPositionSphereMain");
	if (relPosSphereMain)
	{
		relPosSphereMain->QueryAttribute("x", &m_OffsetPositionSphereMain.x);
		relPosSphereMain->QueryAttribute("y", &m_OffsetPositionSphereMain.y);
		relPosSphereMain->QueryAttribute("z", &m_OffsetPositionSphereMain.z);
	}

	m_OffsetPositionBox = Vector3(0.f, 0.f, 0.f);
	const tinyxml2::XMLElement* relPosBox = p_Data->FirstChildElement("OffsetPositionBox");
	if (relPosBox)
	{
		relPosBox->QueryAttribute("x", &m_OffsetPositionBox.x);
		relPosBox->QueryAttribute("y", &m_OffsetPositionBox.y);
		relPosBox->QueryAttribute("z", &m_OffsetPositionBox.z);
	}

	m_OffsetRotation = Vector3(0.f, 0.f, 0.f);
	const tinyxml2::XMLElement* relRot = p_Data->FirstChildElement("OffsetRotation");
	if(relRot)
	{
		queryRotation(relRot, m_OffsetRotation);
	}


}

void PlayerBodyComponent::postInit()
{
	m_Body = m_Physics->createSphere(m_Mass, false, m_Owner->getPosition() + m_OffsetPositionSphereMain, m_RadiusMain);
	m_Physics->addOBBToBody(m_Body, m_Owner->getPosition() + m_OffsetPositionBox, m_HalfsizeBox);
	m_Physics->addSphereToBody(m_Body, m_Owner->getPosition() + m_OffsetPositionSphereMain, m_RadiusAnkle);
	m_Physics->addSphereToBody(m_Body, m_Owner->getPosition() + m_OffsetPositionSphereMain, m_RadiusAnkle);
	m_Physics->setBodyVolumeCollisionResponse(m_Body, 2, false);
	m_Physics->setBodyVolumeCollisionResponse(m_Body, 3, false);
	m_Physics->addSphereToBody(m_Body, m_Owner->getPosition() + m_OffsetPositionSphereHead, m_RadiusHead);

	using namespace DirectX;
	Vector3 ownerRot = m_Owner->getRotation();
	XMMATRIX ownerRotation = XMMatrixRotationRollPitchYaw(ownerRot.y, ownerRot.x, ownerRot.z);
	XMMATRIX compRotation = XMMatrixRotationRollPitchYaw(m_OffsetRotation.y, m_OffsetRotation.x, m_OffsetRotation.z);
	XMMATRIX multRotation = compRotation * ownerRotation;
	XMFLOAT4X4 fMultRotation;
	XMStoreFloat4x4(&fMultRotation, multRotation);
	m_Physics->setBodyRotationMatrix(m_Body, fMultRotation);
	m_Physics->setBodyRotation(m_Body, m_Owner->getRotation());
	m_Physics->setBodyScale(m_Body, m_Scale);
}

void PlayerBodyComponent::serialize(tinyxml2::XMLPrinter& p_Printer) const
{
	p_Printer.OpenElement("PlayerPhysics");
	p_Printer.PushAttribute("RadiusMain", m_RadiusMain);
	p_Printer.PushAttribute("RadiusAnkle", m_RadiusAnkle);
	p_Printer.PushAttribute("RadiusHead", m_RadiusHead);
	p_Printer.PushAttribute("Mass", m_Mass);
	p_Printer.PushAttribute("FallTime", m_FallTime);
	p_Printer.PushAttribute("FallTolerance", m_FallTolerance);
	pushVector(p_Printer, "Scale", m_Scale);
	pushVector(p_Printer, "HalfsizeBox", m_HalfsizeBox);
	pushVector(p_Printer, "OffsetPositionSphereMain", m_OffsetPositionSphereMain);
	pushVector(p_Printer, "OffsetPositionSphereHead", m_OffsetPositionSphereHead);
	pushVector(p_Printer, "OffsetPositionBox", m_OffsetPositionBox);
	pushVector(p_Printer, "OffsetRotation", m_OffsetRotation);
	p_Printer.CloseElement();
}

void PlayerBodyComponent::onUpdate(float p_DeltaTime)
{
	if (m_Owner)
	{
		std::shared_ptr<MovementControlInterface> comp = m_Owner->getComponent<MovementControlInterface>(MovementControlInterface::m_ComponentId).lock();
		std::shared_ptr<RunControlComponent> runComp = std::dynamic_pointer_cast<RunControlComponent>(comp);

		if (runComp)
		{
			if(!m_Physics->getBodyOnSomething(m_Body))
			{
				m_FallTime += p_DeltaTime;

				if(m_FallTime > m_FallTolerance)
				{
					m_FallTime = 0.f;
					runComp->setIsFalling(true);
				}
			}
			else
			{
				runComp->setIsFalling(false);
			}
		}
	}


	m_Owner->setPosition(m_Physics->getBodyPosition(m_Body) - m_OffsetPositionSphereMain);
	Vector3 rotation = m_Owner->getRotation();
	m_Physics->setBodyRotation(m_Body, rotation);
}

void PlayerBodyComponent::setPosition(Vector3 p_Position)
{
	m_Physics->setBodyPosition(m_Body, p_Position + m_OffsetPositionSphereMain);
}

void PlayerBodyComponent::setRotation(Vector3 p_Rotation)
{
	using namespace DirectX;
	Vector3 ownerRot = p_Rotation;
	XMMATRIX ownerRotation = XMMatrixRotationRollPitchYaw(ownerRot.y, ownerRot.x, ownerRot.z);
	XMMATRIX compRotation = XMMatrixRotationRollPitchYaw(m_OffsetRotation.y, m_OffsetRotation.x, m_OffsetRotation.z);
	XMMATRIX multRotation = compRotation * ownerRotation;
	XMFLOAT4X4 fMultRotation;
	XMStoreFloat4x4(&fMultRotation, multRotation);
	m_Physics->setBodyRotationMatrix(m_Body, fMultRotation);
}

BodyHandle PlayerBodyComponent::getBodyHandle() const
{
	return m_Body;
}

Vector3 PlayerBodyComponent::getVelocity() const
{
	return m_Physics->getBodyVelocity(m_Body);
}

bool PlayerBodyComponent::isInAir() const
{
	return m_Physics->getBodyInAir(m_Body);
}
bool PlayerBodyComponent::hasLanded() const
{
	return m_Physics->getBodyLanded(m_Body);
}

bool PlayerBodyComponent::isOnSomething() const
{
	return m_Physics->getBodyOnSomething(m_Body);
}