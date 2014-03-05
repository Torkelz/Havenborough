#pragma once

#include "Components.h"

class FlyingControlComponent : public MovementControlInterface
{
private:
	IPhysics* m_Physics;
	std::weak_ptr<PhysicsInterface> m_PhysicsComp;
	float m_MaxSpeed;
	float m_AccConstant;

	Vector3 m_FlyingDirection;

public:
	FlyingControlComponent();

	void initialize(const tinyxml2::XMLElement* p_Data) override;
	void serialize(tinyxml2::XMLPrinter& p_Printer) const override;
	void postInit() override;

	void move(float p_DeltaTime) override;
	
	Vector3 getLocalDirection() const override;
	void setLocalDirection(const Vector3& p_Direction) override;

	float getMaxSpeed() const override;
	void setMaxSpeed(float p_Speed) override;

	/**
	 * same function as getMaxSpeed().
	 */
	float getMaxSpeedDefault() const override;

	void setPhysics(IPhysics* p_Physics);
};
