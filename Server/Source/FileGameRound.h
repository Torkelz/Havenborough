#pragma once

#include "GameRound.h"
#include "LevelBinaryLoader.h"
#include "CheckpointSystem.h"

#include <DirectXMath.h>
#include <tinyxml2/tinyxml2.h>

class FileGameRound : public GameRound
{
private:
	std::string m_FilePath;
	std::unique_ptr<LevelBinaryLoader> m_FileLoader;
	CheckpointSystem m_CheckpointSystem;
	std::vector<std::pair<User::wPtr, Actor::wPtr>> m_SendHitData;
public:
	void setup() override;
	void setFilePath(std::string p_FilePath);

private:
	void sendLevel() override;
	void updateLogic(float p_DeltaTime) override;
	void sendUpdates() override;
	void playerDisconnected(Player& p_DisconnectedPlayer);

	UpdateObjectData getUpdateData(const Player& p_Player);
};