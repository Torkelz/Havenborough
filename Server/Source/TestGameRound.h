#pragma once

#include "GameRound.h"

#include <tinyxml2/tinyxml2.h>

class TestGameRound : public GameRound
{
private:
	static const float m_PlayerSphereRadius;

	struct TestBox
	{
		uint16_t actorId;

		Vector3 position;
		Vector3 velocity;
		Vector3 rotation;
		Vector3 rotationVelocity;

		Vector3 circleCenter;
		float circleRadius;
		float circleRotationSpeed;
		float circleRotation;
	};

	std::vector<TestBox> m_Boxes;

public:
	void setup() override;

private:
	void sendLevel() override;
	void updateLogic(float p_DeltaTime) override;
	void sendUpdates() override;
	
	void updateBox(TestBox& p_Box, float p_DeltaTime);
	void updatePlayerBox(Player::Box& p_Box, float p_DeltaTime);
	UpdateObjectData getUpdateData(const TestBox& p_Box);
	UpdateObjectData getUpdateData(const Player::Box& p_Box);
	void pushVector(tinyxml2::XMLPrinter& p_Printer, const std::string& p_ElementName, const Vector3& p_Vec);
	std::string getBoxDescription(const TestBox& p_Box);
	std::string getPlayerBoxDescription(const Player::Box& p_Box);
	ObjectInstance getBoxInstance(const TestBox& p_Box, uint16_t p_DescIdx);
	ObjectInstance getBoxInstance(const Player::Box& p_Box, uint16_t p_DescIdx);
};
