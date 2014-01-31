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

	void updateAnimation()
	{
		using namespace DirectX;

		Vector3 tempVector(0.f, 0.f, 0.f);
		bool isInAir = true;
		std::shared_ptr<PhysicsInterface> physComp = m_Owner->getComponent<PhysicsInterface>(PhysicsInterface::m_ComponentId).lock();
		if (physComp)
		{
			tempVector = physComp->getVelocity();
			isInAir = physComp->isInAir();
		}
		XMVECTOR velocity = Vector3ToXMVECTOR(&tempVector, 0.0f);

		tempVector = Vector3(0.f, 0.f, 0.f);
		std::shared_ptr<LookInterface> lookComp = m_Owner->getComponent<LookInterface>(LookInterface::m_ComponentId).lock();
		if (lookComp)
		{
			tempVector = lookComp->getLookForward();
			tempVector.y = 0.f;
		}
		XMVECTOR look = Vector3ToXMVECTOR(&tempVector, 0.0f);
		XMMATRIX rotationInverse = XMMatrixTranspose(XMLoadFloat4x4(&lookComp->getRotationMatrix()));
		velocity = XMVector3Transform(velocity, rotationInverse);
		if (!isInAir)
		{
			// Calculate the weight on the strafe track with some trigonometry.
			//float angle = XMVectorGetX(XMVector3AngleBetweenVectors(look, velocity));
			//changeAnimationWeight(2, cosf(angle));

			// Decide what animation to play on the motion tracks.
			ForwardAnimationState currentForwardState = ForwardAnimationState::IDLE;
			SideAnimationState currentSideState = SideAnimationState::IDLE;

			static const float runLimit = 100.f;
			if (XMVectorGetZ(velocity) > runLimit)
			{
				currentForwardState = ForwardAnimationState::RUNNING_FORWARD;
			}
			else if (XMVectorGetZ(velocity) < -runLimit)
			{
				currentForwardState = ForwardAnimationState::RUNNING_BACKWARD;
			}

			static const float runSideLimit = 100.f;
			if (XMVectorGetX(velocity) > runSideLimit)
			{
				currentSideState = SideAnimationState::RUNNING_RIGHT;
			}
			else if (XMVectorGetX(velocity) < -runSideLimit)
			{
				currentSideState = SideAnimationState::RUNNING_LEFT;
			}

			if (currentForwardState != m_PrevForwardState)
			{
				switch (currentForwardState)
				{
				case ForwardAnimationState::IDLE:
					if (currentSideState == SideAnimationState::IDLE)
					{
						playAnimation("Idle", false);
					}
					break;

				case ForwardAnimationState::RUNNING_FORWARD:
					playAnimation("Run", false);
				}
			}

			if (currentSideState != m_PrevSideState)
			{
				switch (currentSideState)
				{
				case SideAnimationState::IDLE:
					if (currentForwardState == ForwardAnimationState::IDLE)
					{
						playAnimation("Idle", false);
					}
					break;
				}
			}

			m_PrevForwardState = currentForwardState;
			m_PrevSideState = currentSideState;
			m_PrevJumpState = JumpAnimationState::IDLE;
		}
		else
		{
			static const float runLimit = 100.f;
			float flyLimit = 500.0f;
			JumpAnimationState currentJumpState = JumpAnimationState::JUMP;

			if(XMVectorGetY(velocity) < flyLimit)
			{
				currentJumpState = JumpAnimationState::JUMP;
			}
			if(XMVectorGetY(velocity) < -flyLimit)
			{
				currentJumpState = JumpAnimationState::FALLING;
			}

			if (XMVectorGetY(velocity) > m_FallSpeed)
			{
				m_FallSpeed = XMVectorGetY(velocity);
			}

			//if (XMVectorGetY(velocity) < flyLimit && XMVectorGetY(velocity) > -flyLimit && 
			//	m_PrevJumpState != JumpAnimationState::JUMP) //SKA BERO PÅ EN BOOL SOM SÄTTS DÅ BODY SLÅR I MARKEN OAVSETT RIKTNING PÅ Y
			//{
			//	if(m_FallSpeed >= 200.0f)
			//	{
			//		currentJumpState = JumpAnimationState::HARD_LANDING;
			//	}
			//	else
			//	{
			//		currentJumpState = JumpAnimationState::LIGHT_LANDING;
			//	}
			//}

			if (currentJumpState != m_PrevJumpState)
			{
				switch (currentJumpState)
				{
				case JumpAnimationState::IDLE:
					if (currentJumpState == JumpAnimationState::IDLE)
					{
						playAnimation("Idle", false);
					}
					break;

				case JumpAnimationState::JUMP:
					if (m_PrevJumpState != JumpAnimationState::FLYING)
					{
						if(XMVectorGetX(velocity) > runLimit || XMVectorGetZ(velocity) > runLimit)
						{
							playAnimation("RunningJump", false);
							queueAnimation("Falling");
						}
						else
						{
							playAnimation("StandingJump", false);
							queueAnimation("Falling");
						}
					}
					break;

				case JumpAnimationState::FLYING:
					//playAnimation(temp, "Flying", false);
					break;

				case JumpAnimationState::HARD_LANDING:
					playAnimation("HardLanding", false);
					break;

				case JumpAnimationState::LIGHT_LANDING:
					playAnimation("NormalLanding", false);
					break;

				case JumpAnimationState::FALLING:
					//playAnimation(temp, "Falling", false);
					break;

				default: // Just in case so that the code doesn't break, hohohoho
					break;
				}
			}

			m_PrevForwardState = ForwardAnimationState::IDLE;
			m_PrevSideState = SideAnimationState::IDLE;
			m_PrevJumpState = currentJumpState;
		}
	}

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
};