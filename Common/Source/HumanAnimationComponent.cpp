#include "HumanAnimationComponent.h"

void HumanAnimationComponent::updateAnimation()
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
	
	if(!m_ForceMove)
	{
		std::shared_ptr<LookInterface> lookComp = m_Owner->getComponent<LookInterface>(LookInterface::m_ComponentId).lock();
		XMVECTOR look = XMVectorSet(0.f, 0.f, 1.f, 0.f);
		XMMATRIX rotationInverse = XMMatrixTranspose(XMLoadFloat4x4(&lookComp->getRotationMatrix()));
		velocity = XMVector3Transform(velocity, rotationInverse);
		if (!isInAir)
		{
			// Calculate the weight on the strafe track with some trigonometry.
			float angle = XMVectorGetX(XMVector3AngleBetweenVectors(look, velocity));
			changeAnimationWeight(2, 1 - abs(cosf(angle))); // Think again. Negative weights are not allowed.

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

			static const float runSideLimit = 10.f;
			if (XMVectorGetX(velocity) > runSideLimit)
			{
				currentSideState = SideAnimationState::RUNNING_RIGHT;
			}
			else if (XMVectorGetX(velocity) < -runSideLimit)
			{
				currentSideState = SideAnimationState::RUNNING_LEFT;
			}
			else
				changeAnimationWeight(2, 0.0f);

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
					//playAnimation("IdleSide", false);
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
			if (physComp->hasLanded())
			{
				if(m_FallSpeed >= 1000.0f)
				{
					currentJumpState = JumpAnimationState::HARD_LANDING;
				}
				else
				{
					if(m_FallSpeed > 0.0f)
						currentJumpState = JumpAnimationState::LIGHT_LANDING;
				}
				m_FallSpeed = 0.0f;
			}

			if (currentJumpState != m_PrevJumpState)
			{
				switch (currentJumpState)
				{
				case JumpAnimationState::HARD_LANDING:
					playAnimation("HardLanding", true);
					if (XMVectorGetZ(velocity) > runLimit)
						queueAnimation("Run");
					else
						queueAnimation("Idle2");
					break;

				case JumpAnimationState::LIGHT_LANDING:
					playAnimation("NormalLanding", true);
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
							playAnimation("RunningJump", true);
							queueAnimation("Falling");
						}
						else
						{
							playAnimation("StandingJump", true);
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

				default: // Just in case, so that the code doesn't break, hohohoho
					break;
				}
			}

			m_PrevForwardState = ForwardAnimationState::WALKING_FORWARD;
			m_PrevSideState = SideAnimationState::IDLE;
			m_PrevJumpState = currentJumpState;
		}
	}
}

void HumanAnimationComponent::updateIKJoints()
{
	using namespace DirectX;
	if(m_ForceMove)
	{
		if(m_ClimbId == "Climb1")
		{

		}
		else if(m_ClimbId == "Climb2")
		{
			XMVECTOR reachPointR;
			reachPointR = XMLoadFloat3(&m_CenterReachPos) + (XMLoadFloat3(&m_EdgeOrientation) * 0);
			Vector3 vReachPointR = Vector4(reachPointR).xyz();
			applyIK_ReachPoint("RightArm", vReachPointR);
		}

		else if(m_ClimbId == "Climb3")
		{
			XMVECTOR reachPoint;
			reachPoint = XMLoadFloat3(&m_CenterReachPos) + (XMLoadFloat3(&m_EdgeOrientation) * 20);
			Vector3 vReachPoint = Vector4(reachPoint).xyz();
			applyIK_ReachPoint("RightArm", vReachPoint);

			reachPoint = XMLoadFloat3(&m_CenterReachPos) - (XMLoadFloat3(&m_EdgeOrientation) * 20);
			vReachPoint = Vector4(reachPoint).xyz();
			applyIK_ReachPoint("LeftArm", vReachPoint);
		}
		
		else if(m_ClimbId == "Climb4")
		{
			XMVECTOR reachPoint;
			reachPoint = XMLoadFloat3(&m_CenterReachPos) + (XMLoadFloat3(&m_EdgeOrientation) * 20);
			Vector3 vReachPoint = Vector4(reachPoint).xyz();
			applyIK_ReachPoint("RightArm", vReachPoint);

			reachPoint = XMLoadFloat3(&m_CenterReachPos) - (XMLoadFloat3(&m_EdgeOrientation) * 20);
			vReachPoint = Vector4(reachPoint).xyz();
			applyIK_ReachPoint("LeftArm", vReachPoint);
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
				applyIK_ReachPoint("LeftLeg", Vector4ToXMFLOAT3(&hit.colPos));

				DirectX::XMFLOAT3 anklePos = getJointPos("L_Ankle");
				DirectX::XMFLOAT3 toePos = getJointPos("L_FootBase");
				DirectX::XMVECTOR vAnkle = DirectX::XMLoadFloat3(&anklePos);
				DirectX::XMVECTOR vToe = DirectX::XMLoadFloat3(&toePos);

				vToe = vToe - vAnkle;
				vToe.m128_f32[1] = 0.f;
			
				vToe = DirectX::XMVector3Normalize(vToe);
				vToe *= 20.0f;
				vToe += vAnkle;
				vToe.m128_f32[1] = hit.colPos.y;
				hit.colPos = vToe;

				applyIK_ReachPoint("LeftFoot", Vector4ToXMFLOAT3(&hit.colPos));
			}
			if(hit.IDInBody == 3 && hit.colType != Type::SPHEREVSSPHERE && hit.collider == m_Owner->getBodyHandles()[0])
			{
				hit.colPos.y += 5.0f;
				applyIK_ReachPoint("RightLeg", Vector4ToXMFLOAT3(&hit.colPos));

				DirectX::XMFLOAT3 anklePos = getJointPos("R_Ankle");
				DirectX::XMFLOAT3 toePos = getJointPos("R_FootBase");
				DirectX::XMVECTOR vAnkle = DirectX::XMLoadFloat3(&anklePos);
				DirectX::XMVECTOR vToe = DirectX::XMLoadFloat3(&toePos);

				vToe = vToe - vAnkle;
				vToe.m128_f32[1] = 0.f;
			
				vToe = DirectX::XMVector3Normalize(vToe);
				vToe *= 20.0f;
				vToe += vAnkle;
				vToe.m128_f32[1] = hit.colPos.y;
				hit.colPos = vToe;

				applyIK_ReachPoint("RightFoot", Vector4ToXMFLOAT3(&hit.colPos));
			}
		}
	}
}
