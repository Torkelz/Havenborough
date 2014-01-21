#pragma once

#include "GameRound.h"

#include <memory>
#include <string>

class GameRoundFactory
{
public:
	GameRound::ptr createRound(const std::string& p_GameType);
};
