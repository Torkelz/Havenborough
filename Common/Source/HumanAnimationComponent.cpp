#include "HumanAnimationComponent.h"
#include "RunControlComponent.h"
#include "Logger.h"

void HumanAnimationComponent::updateAnimation()
{
	using namespace DirectX;

	Vector3 tempVector(0.f, 0.f, 0.f);
	bool isInAir = true;
	bool isFalling = false;
	bool isJumping = false;
	bool isOnSomething = false;
	std::shared_ptr<PhysicsInterface> physComp = m_Owner->getComponent<PhysicsInterface>(PhysicsInterface::m_ComponentId).lock();
	if (physComp)
	{
		tempVector = physComp->getVelocity();
		isInAir = physComp->isInAir();
		isOnSomething = physComp->isOnSomething();
	}
	XMVECTOR velocity = Vector3ToXMVECTOR(&tempVector, 0.0f);
	std::shared_ptr<MovementControlInterface> comp = m_Owner->getComponent<MovementControlInterface>(MovementControlInterface::m_ComponentId).lock();
	std::shared_ptr<RunControlComponent> runComp = std::dynamic_pointer_cast<RunControlComponent>(comp);
	if(runComp)
	{
		isFalling = runComp->getIsFalling();
		isJumping = runComp->getIsJumping();
	}

	if(!m_ForceMove)
	{
		std::shared_ptr<LookInterface> lookComp = m_Owner->getComponent<LookInterface>(LookInterface::m_ComponentId).lock();
		XMVECTOR look = XMVectorSet(0.f, 0.f, 1.f, 0.f);
		XMMATRIX rotationInverse = XMMatrixTranspose(XMLoadFloat4x4(&lookComp->getRotationMatrix()));
		velocity = XMVector3Transform(velocity, rotationInverse);

		// Calculate the weight on the strafe track with some trigonometry.
		float angle = XMVectorGetX(XMVector3AngleBetweenVectors(look, velocity));
		changeAnimationWeight(2, 1 - abs(cosf(angle)));
		if (!isInAir)
		{
			m_QueuedFalling = false;
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
			else if (XMVectorGetX(velocity) >= -10.f && XMVectorGetX(velocity) <= 10.f )
			{
				changeAnimationWeight(2, 0.0f);
			}

			if (currentForwardState != m_PrevForwardState)
			{
				switch (currentForwardState)
				{
				case ForwardAnimationState::IDLE:
					playAnimation("Idle2", false);
					break;
				case ForwardAnimationState::RUNNING_FORWARD:
					playAnimation("Run", false);
					break;
				case ForwardAnimationState::RUNNING_BACKWARD:
					playAnimation("BackPaddle", false);
					break;
				}
			}

			if (currentSideState != m_PrevSideState)
			{
				switch (currentSideState)
				{
				case SideAnimationState::IDLE:
					playAnimation("IdleSide", false);
					break;

				case SideAnimationState::RUNNING_LEFT:
					playAnimation("SideStepLeft", false);
					break;

				case SideAnimationState::RUNNING_RIGHT:
					playAnimation("SideStepRight", false);
					break;
				}
			}

			JumpAnimationState currentJumpState = JumpAnimationState::JUMP;
			if (physComp->isOnSomething())
			{
				
				if(m_FallSpeed >= 1400.0f)
				{
					currentJumpState = JumpAnimationState::HARD_LANDING;
				}
				else
				{
					if(m_FallSpeed > 500.0f)
					{
						currentJumpState = JumpAnimationState::LIGHT_LANDING;
					}
				}
				m_FallSpeed = 0.0f;
			}

			if (currentJumpState != m_PrevJumpState)
			{
				switch (currentJumpState)
				{
				case JumpAnimationState::HARD_LANDING:
						playAnimation("HardLanding", false);
						queueAnimation("Idle2");
						m_Crash = true;
						break;
				case JumpAnimationState::LIGHT_LANDING:
					playAnimation("BodyLand", false);
					if (XMVectorGetZ(velocity) > runLimit)
						queueAnimation("Run");
					else
						queueAnimation("Idle2");
					break;

				default: // Just in case, so that the code doesn't break, hohohoho
					break;
				}
			}

			m_PrevForwardState = currentForwardState;
			m_PrevSideState = currentSideState;
			m_PrevJumpState = JumpAnimationState::IDLE;
		}
		else if(isFalling || isJumping)
		{
			float weight = 1 - (abs(cosf(angle)));
			if(weight > 0.8f)
				int lol = 42;
			static const float runLimit = 100.f;
			float flyLimit = 500.0f;
			JumpAnimationState currentJumpState = JumpAnimationState::JUMP;

			if(XMVectorGetY(velocity) < flyLimit)
			{
				currentJumpState = JumpAnimationState::JUMP;
			}
			if(XMVectorGetY(velocity) < -flyLimit)
			{
				if(m_PrevJumpState != JumpAnimationState::FALLING)
				currentJumpState = JumpAnimationState::FALLING;
			}

			if (fabs(XMVectorGetY(velocity)) > m_FallSpeed)
			{
				m_FallSpeed = fabs(XMVectorGetY(velocity));
			}

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
						if(XMVectorGetZ(velocity) > runLimit && isJumping)
						{
							playAnimation("RunningJump", true);
							queueAnimation("Falling");
							m_QueuedFalling = true;
						}
						else if (XMVectorGetX(velocity) > runLimit && isJumping)
						{
							playAnimation("SideJumpRight", false);
							queueAnimation("FallingSide");
							m_QueuedFalling = true;
						}
						else if (XMVectorGetX(velocity) < -runLimit && isJumping)
						{
							playAnimation("SideJumpLeft", false);
							queueAnimation("FallingSide");
							m_QueuedFalling = true;
						}
						else if(isJumping)
						{
							playAnimation("StandingJump", true);
							queueAnimation("Falling");
							m_QueuedFalling = true;
						}
						else
						{
							if(!m_QueuedFalling)
							{
								m_QueuedFalling = true;
								playAnimation("Falling", false);
							}
							currentJumpState = JumpAnimationState::FALLING;
						}
					}
					break;
				case JumpAnimationState::FLYING:
					//playAnimation(temp, "Flying", false);
					break;
				case JumpAnimationState::HARD_LANDING:
						playAnimation("HardLanding", false);
						queueAnimation("Idle2");
						m_Crash = true;
					break;

				case JumpAnimationState::LIGHT_LANDING:
					playAnimation("BodyLand", false);
					if (XMVectorGetZ(velocity) > runLimit)
						queueAnimation("Run");
					else
						queueAnimation("Idle2");
					break;
				case JumpAnimationState::FALLING:
					//playAnimation("Falling", true);
					break;
				default: // Just in case, so that the code doesn't break, hohohoho
					break;
				}
			}
			
			if(isFalling)
				m_PrevForwardState = ForwardAnimationState::WALKING_FORWARD;
			else
				m_PrevForwardState = ForwardAnimationState::RUNNING_FORWARD;

			m_PrevSideState = SideAnimationState::IDLE;
			m_PrevJumpState = currentJumpState;
		}
	}
	else
		m_FallSpeed = 0.f;

	if(m_Crash)
		m_PrevForwardState = ForwardAnimationState::IDLE;
	if(m_ForceMove)
		m_PrevForwardState = ForwardAnimationState::RUNNING_FORWARD;
}

void HumanAnimationComponent::updateIKJoints(float dt)
{
	using namespace DirectX;
	if(m_ForceMove)
	{
		m_Shell.updateTimeStamps(dt);
		if(m_ClimbId == "Climb1")
		{
		}
		else if(m_ClimbId == "Climb2")
		{
			if(m_Shell.m_Grabs.at("RightArm").m_Active)
			{
				XMVECTOR reachPoint;
				reachPoint = XMLoadFloat3(&m_CenterReachPos) + (XMLoadFloat3(&m_EdgeOrientation) * m_Shell.m_Grabs.at("RightArm").m_Position);
				Vector3 vReachPointR = Vector4(reachPoint).xyz();
				applyIK_ReachPoint("RightArm", vReachPointR, m_Shell.m_Weight);
			}
		}

		else if(m_ClimbId == "Climb3")
		{
			XMVECTOR reachPoint;
			Vector3 vReachPoint;
			if(m_Shell.m_Grabs.at("RightArm").m_Active)
			{
				reachPoint = XMLoadFloat3(&m_CenterReachPos) + (XMLoadFloat3(&m_EdgeOrientation) * m_Shell.m_Grabs.at("RightArm").m_Position);
				vReachPoint = Vector4(reachPoint).xyz();
				applyIK_ReachPoint("RightArm", vReachPoint, m_Shell.m_Weight);
			}
			if(m_Shell.m_Grabs.at("LeftArm").m_Active)
			{
				reachPoint = XMLoadFloat3(&m_CenterReachPos) + (XMLoadFloat3(&m_EdgeOrientation) * m_Shell.m_Grabs.at("LeftArm").m_Position);
				vReachPoint = Vector4(reachPoint).xyz();
				applyIK_ReachPoint("LeftArm", vReachPoint, m_Shell.m_Weight);
			}
		}
		
		else if(m_ClimbId == "Climb4")
		{
			XMVECTOR reachPoint;
			Vector3 vReachPoint;
			if(m_Shell.m_Grabs.at("RightArm").m_Active)
			{
				reachPoint = XMLoadFloat3(&m_CenterReachPos) + (XMLoadFloat3(&m_EdgeOrientation) * m_Shell.m_Grabs.at("RightArm").m_Position);
				vReachPoint = Vector4(reachPoint).xyz();
				applyIK_ReachPoint("RightArm", vReachPoint, m_Shell.m_Weight);
			}

			if(m_Shell.m_Grabs.at("LeftArm").m_Active)
			{
				reachPoint = XMLoadFloat3(&m_CenterReachPos) + (XMLoadFloat3(&m_EdgeOrientation) * m_Shell.m_Grabs.at("LeftArm").m_Position);
				vReachPoint = Vector4(reachPoint).xyz();
				applyIK_ReachPoint("LeftArm", vReachPoint, m_Shell.m_Weight);
			}
		}
	}
	else
	{
		int hitsSize = m_Physics->getHitDataSize();
		for(int i = 0; i < hitsSize; i++)
		{
			HitData hit = m_Physics->getHitDataAt(i);

			
			if(hit.IDInBody == 2 && hit.colType != Type::SPHEREVSSPHERE && hit.collider == m_Owner->getBodyHandles()[0])
			{
				hit.colPos.y += 5.0f;
				applyIK_ReachPoint("LeftLeg", Vector4ToXMFLOAT3(&hit.colPos), 1.0f);

				DirectX::XMFLOAT3 anklePos = getJointPos("L_Ankle");
				DirectX::XMFLOAT3 toePos = getJointPos("L_FootBase");
				DirectX::XMVECTOR vAnkle = DirectX::XMLoadFloat3(&anklePos);
				DirectX::XMVECTOR vToe = DirectX::XMLoadFloat3(&toePos);

				vToe = vToe - vAnkle;
				vToe.m128_f32[1] = 0.f;
			
				if (m_Dzala)
				{
					vToe = DirectX::XMVector3Normalize(vToe);
					vToe *= 20.0f;
					vToe += vAnkle;
					hit.colPos = vToe;
				}
				else
				{
					vToe *= 2.0f;
					vToe += vAnkle;
					vToe.m128_f32[1] -= 10.0f;
					hit.colPos = vToe;
				}

				applyIK_ReachPoint("LeftFoot", Vector4ToXMFLOAT3(&hit.colPos), 1.0f);
			}
			if(hit.IDInBody == 3 && hit.colType != Type::SPHEREVSSPHERE && hit.collider == m_Owner->getBodyHandles()[0])
			{
				hit.colPos.y += 5.0f;
				applyIK_ReachPoint("RightLeg", Vector4ToXMFLOAT3(&hit.colPos), 1.0f);

				DirectX::XMFLOAT3 anklePos = getJointPos("R_Ankle");
				DirectX::XMFLOAT3 toePos = getJointPos("R_FootBase");
				DirectX::XMVECTOR vAnkle = DirectX::XMLoadFloat3(&anklePos);
				DirectX::XMVECTOR vToe = DirectX::XMLoadFloat3(&toePos);

				float ydiff = anklePos.y - toePos.y;
								
				vToe = vToe - vAnkle;
				vToe.m128_f32[1] = 0.f;
				
				if (m_Dzala)
				{
					vToe = DirectX::XMVector3Normalize(vToe);
					vToe *= 20.0f;
					vToe += vAnkle;
					hit.colPos = vToe;
				}
				else
				{
					vToe *= 2.0f;
					vToe += vAnkle;
					vToe.m128_f32[1] -= 10.0f;
					hit.colPos = vToe;
				}
				
				applyIK_ReachPoint("RightFoot", Vector4ToXMFLOAT3(&hit.colPos), 1.0f);
			}
		}
	}
}
