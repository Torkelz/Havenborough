#include "ActorComponent.h"
#include "Components.h"

class PlayerBodyComponent : public PhysicsInterface
{
private:
	BodyHandle m_Body;
	IPhysics* m_Physics;
	float m_RadiusMain;
	float m_RadiusAnkle;
	float m_RadiusHead;
	float m_Mass;
	float m_FallTime;
	float m_FallTolerance;
	Vector3 m_OffsetPositionSphereMain;
	Vector3 m_OffsetPositionSphereHead;
	Vector3 m_OffsetPositionBox;
	Vector3 m_OffsetRotation;
	Vector3 m_HalfsizeBox;
	Vector3 m_Scale;
public:
	~PlayerBodyComponent() override;
	void setPhysics(IPhysics* p_Physics);
	void initialize(const tinyxml2::XMLElement* p_Data) override;
	void postInit() override;
	void serialize(tinyxml2::XMLPrinter& p_Printer) const override;
	void onUpdate(float p_DeltaTime) override;
	void setPosition(Vector3 p_Position) override;
	void setRotation(Vector3 p_Rotation) override;
	BodyHandle getBodyHandle() const override;
	Vector3 getVelocity() const override;
	bool isInAir() const override;
	bool hasLanded() const override;
	bool isOnSomething() const override;
	void resetFallTime();
};