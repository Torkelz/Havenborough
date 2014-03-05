#pragma once

#include "Components.h"

class SplineControlComponent : public MovementControlInterface
{
private:
	IPhysics* m_Physics;
	std::weak_ptr<PhysicsInterface> m_PhysicsComp;
	std::weak_ptr<LookInterface> m_LookComp;
	float m_MaxSpeed;
	float m_AccConstant;

	Vector3 m_FlyingDirection;

	bool m_SplineActivated;
	unsigned int m_SplineSequence;

	struct sequence
	{
		std::vector<Vector3> m_Positions;
		std::vector<Vector3> m_LookForward;
		std::vector<Vector3> m_LookUp;
		std::vector<float> m_Time;
	};
	std::vector<sequence> m_Sequences;

	float m_CurrentTime;
	float m_RecTimeBetweenPoint;
	unsigned int m_CurrentPoint;

public:
	SplineControlComponent();

	void initialize(const tinyxml2::XMLElement* p_Data) override;
	void serialize(tinyxml2::XMLPrinter& p_Printer) const override;
	void postInit() override;

	void move(float p_DeltaTime) override;

	Vector3 getLocalDirection() const override;
	void setLocalDirection(const Vector3& p_Direction) override;

	float getMaxSpeed() const override;
	void setMaxSpeed(float p_Speed) override;
	float getMaxSpeedDefault() const override;

	void setPhysics(IPhysics* p_Physics);

	void recordPoint();
	void removePreviousPoint();
	void clearSequence();
private:
	void runSpline(int p_Sequence);
	
	void savePath(const std::string &p_Filename);
	void loadPath(const std::string &p_Filename);

	void newSequence();
	void currentSequence();
	void selectSequence(unsigned int p_Sequence);

	Vector3 catMullRom(const std::vector<Vector3> &p_Path, unsigned int p_CurrPoint, float p_Percentage);

	void defaultMove(float p_DeltaTime);
};
