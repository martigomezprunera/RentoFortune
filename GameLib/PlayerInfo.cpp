#include "PlayerInfo.h"
#include <random>


PlayerInfo::PlayerInfo()
{
	casilla = 0;
	money = 10;
	isYourTurn = false;
	std::vector<int> myProperties;
}

PlayerInfo::~PlayerInfo()
{
}
