#pragma once
#include <SFML\Graphics.hpp>
#include <SFML/Network.hpp>
#include <iostream>

class PlayerInfo
{
public:
	int id;
	std::string name;
	sf::Vector2i position;
	int money;
	bool isYourTurn;
	int casilla;

	PlayerInfo();
	~PlayerInfo();
};