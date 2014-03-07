#pragma once

#include "GameRound.h"
#include "InstanceBinaryLoader.h"

#include <DirectXMath.h>
#include <tinyxml2/tinyxml2.h>

class FileGameRound : public GameRound
{
private:
	std::string m_FilePath;
	std::unique_ptr<InstanceBinaryLoader> m_FileLoader;
	std::vector<std::pair<Player::ptr, Actor::wPtr>> m_SendHitData;
	std::vector<std::pair<std::string, float>> m_ResultList;
	bool m_ResultListUpdated;

	std::vector<Player::ptr> m_PlayerPositionList;

	float m_Time;
public:
	void setup() override;
	void setFilePath(std::string p_FilePath);

private:
	void sendLevel() override;
	void updateLogic(float p_DeltaTime) override;
	void handleExtraPackage(Player::ptr p_Player, Package p_Package) override;
	void sendUpdates() override;
	void playerDisconnected(Player::ptr p_DisconnectedPlayer) override;

	UpdateObjectData getUpdateData(const Player::ptr p_Player);
	std::string getExtraData(const Player::ptr p_Player);
	Player::ptr findPlayer(BodyHandle p_Body);
	Actor::ptr findActor(BodyHandle p_Body);

	void rearrangePlayerPosition();
	unsigned int getPlayerPos(Actor::Id p_Player);

	unsigned int countPlayersRacing() const;
};