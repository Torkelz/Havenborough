#pragma once

#include "Components.h"

class RunControlComponent : public MovementControlInterface
{
private:
	IPhysics* m_Physics;
	std::weak_ptr<PhysicsInterface> m_PhysicsComp;
	float m_MaxSpeed;
	float m_MaxSpeedDefault;
	float m_MaxSpeedCurrent;
	float m_MaxSpeedAccelerationFactor; // factor to increase maxspeed
	float m_AccConstant;

	Vector3 m_RunningDirection;
	bool m_IsJumping;
	Vector3 m_GroundNormal;

public:
	RunControlComponent();

	void initialize(const tinyxml2::XMLElement* p_Data) override;
	void serialize(tinyxml2::XMLPrinter& p_Printer) const override;
	void postInit() override;

	void move(float p_DeltaTime) override;
	
	Vector3 getLocalDirection() const override;
	void setLocalDirection(const Vector3& p_Direction) override;

	float getMaxSpeed() const override;
	void setMaxSpeed(float p_Speed) override;

	float getMaxSpeedDefault() const override;
	void setMaxSpeedDefault(float p_Factor);

	float getMaxSpeedCurrent() const;

	void setAccelerationConstant(float p_Acceleration);
	void setMaxSpeedAccelerationFactor(float p_Factor);

	bool getIsJumping() const;
	void setIsJumping(bool p_Jumping);
	
	Vector3 getGroundNormal() const;
	void setGroundNormal(const Vector3& p_Normal);

	void setPhysics(IPhysics* p_Physics);
};
