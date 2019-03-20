#include "PlayerInfo.h"
#include <random>


PlayerInfo::PlayerInfo()
{
	casilla = 0;
	money = 40;
	isYourTurn = false;
	std::vector<int> myProperties;
}

PlayerInfo::~PlayerInfo()
{
}
