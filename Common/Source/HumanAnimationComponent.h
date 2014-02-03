#pragma once

#include "ActorComponent.h"
#include "Components.h"
#include "EventManager.h"

class HumanAnimationComponent : public AnimationInterface
{
private:
	enum class ForwardAnimationState
	{
		IDLE,
		WALKING_FORWARD,
		RUNNING_FORWARD,
		WALKING_BACKWARD,
		RUNNING_BACKWARD,
	};

	enum class SideAnimationState
	{
		IDLE,
		WALKING_LEFT,
		RUNNING_LEFT,
		WALKING_RIGHT,
		RUNNING_RIGHT,
	};

	enum class JumpAnimationState
	{
		IDLE,
		JUMP,
		FLYING,
		FALLING,
		LIGHT_LANDING,
		HARD_LANDING,
	};

	ForwardAnimationState m_PrevForwardState;
	SideAnimationState m_PrevSideState;
	JumpAnimationState m_PrevJumpState;
	float m_FallSpeed;

	std::weak_ptr<ModelComponent> m_Model;
	EventManager* m_EventManager;

public:
	void initialize(const tinyxml2::XMLElement* p_Data) override
	{
		m_FallSpeed = 0.f;
		m_PrevForwardState = ForwardAnimationState::IDLE;
		m_PrevSideState = SideAnimationState::IDLE;
		m_PrevJumpState = JumpAnimationState::IDLE;
	}

	void postInit()
	{
		m_Model = m_Owner->getComponent<ModelComponent>(ModelInterface::m_ComponentId);
		m_EventManager = m_Owner->getEventManager();
		playAnimation("Idle", false);
	}

	void onUpdate(float p_DeltaTime) override
	{
		updateAnimation();
	}

	void serialize(tinyxml2::XMLPrinter& p_Printer) const override
	{
		p_Printer.OpenElement("HumanAnimation");
		p_Printer.CloseElement();
	}

	void updateAnimation();

	void playAnimation(std::string p_AnimationName, bool p_Override)
	{
		std::shared_ptr<ModelComponent> comp = m_Model.lock();
		if (comp)
		{
			m_EventManager->queueEvent(IEventData::Ptr(new PlayAnimationEventData(comp->getId(), p_AnimationName, p_Override)));
		}
	}

	void queueAnimation(std::string p_AnimationName)
	{
		std::shared_ptr<ModelComponent> comp = m_Model.lock();
		if (comp)
		{
			m_EventManager->queueEvent(IEventData::Ptr(new QueueAnimationEventData(comp->getId(), p_AnimationName)));
		}
	}

	void changeAnimationWeight(int p_Track, float p_Weight)
	{
		std::shared_ptr<ModelComponent> comp = m_Owner->getComponent<ModelComponent>(ModelInterface::m_ComponentId).lock();
		if (comp)
		{
			m_Owner->getEventManager()->queueEvent(IEventData::Ptr(new ChangeAnimationWeightEventData(comp->getId(), p_Track, p_Weight)));
		}
	}
};