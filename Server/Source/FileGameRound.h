#pragma once

#include "GameRound.h"
#include "LevelBinaryLoader.h"

#include <DirectXMath.h>
#include <tinyxml2/tinyxml2.h>

class FileGameRound : public GameRound
{
private:
	std::string m_FilePath;
	std::unique_ptr<LevelBinaryLoader> m_FileLoader;
	std::vector<std::pair<Player::ptr, Actor::wPtr>> m_SendHitData;
public:
	void setup() override;
	void setFilePath(std::string p_FilePath);

private:
	void sendLevel() override;
	void updateLogic(float p_DeltaTime) override;
	void sendUpdates() override;
	void playerDisconnected(Player::ptr p_DisconnectedPlayer);

	UpdateObjectData getUpdateData(const Player::ptr p_Player);
	std::string getExtraData(const Player::ptr p_Player);
	Player::ptr findPlayer(BodyHandle p_Body);
	Actor::ptr findActor(BodyHandle p_Body);
};