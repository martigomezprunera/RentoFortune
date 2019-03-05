#include "PlayerInfo.h"
#include <random>


PlayerInfo::PlayerInfo()
{
	position.x = std::rand() % 10;
	position.y = std::rand() % 10;
	money = 1250;
	isYourTurn = false;
}

PlayerInfo::~PlayerInfo()
{
}
