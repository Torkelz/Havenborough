#pragma once

#include "Animation.h"
#include "AnimationLoader.h"
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

	Animation m_Animation;
	ForwardAnimationState m_PrevForwardState;
	SideAnimationState m_PrevSideState;
	JumpAnimationState m_PrevJumpState;
	float m_FallSpeed;

	std::weak_ptr<ModelComponent> m_Model;
	EventManager* m_EventManager;
	ResourceManager* m_ResourceManager;
	std::string m_AnimationName;
	int m_AnimationResource;
	AnimationLoader* m_AnimationLoader;

public:
	~HumanAnimationComponent()
	{
		m_ResourceManager->releaseResource(m_AnimationResource);
	}

	void initialize(const tinyxml2::XMLElement* p_Data) override
	{
		m_FallSpeed = 0.f;
		m_PrevForwardState = ForwardAnimationState::IDLE;
		m_PrevSideState = SideAnimationState::IDLE;
		m_PrevJumpState = JumpAnimationState::IDLE;

		const char* resourceName = p_Data->Attribute("Animation");
		if (!resourceName)
		{
			throw CommonException("Human animation component must have animation", __LINE__, __FILE__);
		}

		m_AnimationName = resourceName;
		m_AnimationResource = m_ResourceManager->loadResource("animation", m_AnimationName);
		m_Animation.setAnimationData(m_AnimationLoader->getAnimationData(m_AnimationName.c_str()));
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
		m_Animation.updateAnimation(p_DeltaTime);

		std::shared_ptr<ModelComponent> comp = m_Model.lock();
		if (comp)
		{
			m_Owner->getEventManager()->queueEvent(IEventData::Ptr(new UpdateAnimationEventData(comp->getId(), m_Animation.getFinalTransform())));
		}
	}

	void serialize(tinyxml2::XMLPrinter& p_Printer) const override
	{
		p_Printer.OpenElement("HumanAnimation");
		p_Printer.PushAttribute("Animation", m_AnimationName.c_str());
		p_Printer.CloseElement();
	}
	/**
	 * Set the resource manager for the component.
	 *
	 * @param p_ResourceManager the resource manager to use
	 */
	void setResourceManager(ResourceManager* p_ResourceManager)
	{
		m_ResourceManager = p_ResourceManager;
	}

	void setAnimationLoader(AnimationLoader* p_AnimationLoader)
	{
		m_AnimationLoader = p_AnimationLoader;
	}

	void updateAnimation();

	void playAnimation(std::string p_AnimationName, bool p_Override) override
	{
		m_Animation.playClip(p_AnimationName, p_Override);
	}

	void queueAnimation(std::string p_AnimationName) override
	{
		m_Animation.queueClip(p_AnimationName);
	}

	void changeAnimationWeight(int p_Track, float p_Weight) override
	{
		m_Animation.changeWeight(p_Track, p_Weight);
	}

	void applyIK_ReachPoint(const std::string& p_GroupName, Vector3 p_Target) override
	{
		m_Animation.applyIK_ReachPoint(p_GroupName, p_Target, m_Owner->getWorldMatrix());
		
		std::shared_ptr<ModelComponent> comp = m_Model.lock();
		if (comp)
		{
			m_Owner->getEventManager()->queueEvent(IEventData::Ptr(new UpdateAnimationEventData(comp->getId(), m_Animation.getFinalTransform())));
		}
	}
};