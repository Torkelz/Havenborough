#pragma once

#include "GameRound.h"

#include <tinyxml2/tinyxml2.h>

class TestGameRound : public GameRound
{
private:
	static const float m_PlayerSphereRadius;

	std::vector<Actor::wPtr> m_Boxes;

public:
	void setup() override;

private:
	void sendLevel() override;
	void updateLogic(float p_DeltaTime) override;
	void sendUpdates() override;
	void playerDisconnected(Player& p_DisconnectedPlayer) override;
	
	UpdateObjectData getUpdateData(const Actor::ptr p_Box);
	UpdateObjectData getUpdateData(const Player& p_Player);
	std::string getExtraData(const Player& p_Player);
};
