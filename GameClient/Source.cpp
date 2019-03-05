#pragma once
#include <PlayerInfo.h>
#include <SFML\Graphics.hpp>
#include <string>
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <list>
#include <SFML/Network.hpp>

//CONSTANTES
#define PORT 50000
#define IP "127.0.0.1"

//VARIABLES GLOBALES
bool running = true;
sf::Socket::Status status;
sf::Mutex mtx;

//ENUM (ORDENES DEL CLIENTE)
enum Ordenes
{
	CreatePlayer
};

//MENSAJES DEL SERVIDOR
void RecepcionMensaje(sf::TcpSocket* sock)
{
	sf::Packet packReceive;
	while (running)
	{
		sf::Packet packReceive;
		int order;
		sock->receive(packReceive);
		packReceive >> order;

		switch (order)
		{
		case 0:
			std::cout << "Empezamos juego" << std::endl;
			break;
		case 1:
			break;
		case 2:
			break;
		}
	}
}

//OVERCHARGED FUNCTIONS (PLAYER INFO)
sf::Packet& operator <<(sf::Packet& packet, const PlayerInfo& playerInfo)
{
	return packet << playerInfo.name << playerInfo.position.x << playerInfo.position.y << playerInfo.money << playerInfo.isYourTurn;
}
sf::Packet& operator >>(sf::Packet& packet, PlayerInfo& playerInfo)
{
	return packet >> playerInfo.name >> playerInfo.position.x >> playerInfo.position.y >> playerInfo.money >> playerInfo.isYourTurn;
}

//OVERCHARGED FUNCTIONS (ENUM CLASS)
sf::Packet& operator <<(sf::Packet& packet, const Ordenes& orders)
{
	int option = static_cast<int>(orders);
	return packet << option;
}
sf::Packet& operator >>(sf::Packet& packet, Ordenes& orders)
{
	int option = static_cast<int>(orders);
	return packet >> option;
}

int main()
{
	//VARIABLES JUEGO
	std::string aliasName;

	//VARIABLES CONEXION
	sf::TcpSocket sock;
	sf::TcpSocket::Status status;
	sf::Packet pack;

	status = sock.connect(IP, PORT);
	if (status == sf::Socket::Status::Done)
	{
		std::cout << "Nos hemos conectado al servidor" << std::endl;

		//THREAD PARA EL RECEIVE
		std::thread t(&RecepcionMensaje, &sock);
		t.detach();
	}

	//CARGAMOS FUENTES
	sf::Font courFont;
	if (!courFont.loadFromFile("cour.ttf"))
	{
		std::cout << "Error de fuente cargada" << std::endl;
	}

	//WINDOW
	sf::Vector2i screenDimensions(1000, 800);
	sf::RenderWindow window;
	window.create(sf::VideoMode(screenDimensions.x, screenDimensions.y), "Chat");

	//FONT
	sf::Font fontCourbd;

	if (!fontCourbd.loadFromFile("courbd.ttf"))
	{
		std::cout << "Can't load the font file" << std::endl;
	}

	//LOBBY
	/////////////// --- TEXT --- //////////////////
	//--- TITLE ---//
	sf::Text lobbyText("Rento Fortune", fontCourbd, 34);
	lobbyText.setFillColor(sf::Color(255, 255, 255));
	lobbyText.setPosition(500 - 140, 400 - 150);
	lobbyText.setStyle(sf::Text::Bold);

	//--- EDITTEXT TITLE ---//
	sf::Text editTextTitle("Put your Alias", fontCourbd, 26);
	editTextTitle.setFillColor(sf::Color(255, 255, 255));
	editTextTitle.setPosition(500 - 120, 400 - 100);
	editTextTitle.setStyle(sf::Text::Bold);

	////////////// --- RECTANGLE --- ////////////
	sf::RectangleShape rectangleLobby(sf::Vector2f(200, 40));
	rectangleLobby.setFillColor(sf::Color(0, 51, 204, 240));
	rectangleLobby.setOutlineThickness(5);
	rectangleLobby.setOutlineColor(sf::Color(255, 255, 255, 255));
	rectangleLobby.setPosition(500 - 200, 400 - 150);
	rectangleLobby.setSize(sf::Vector2f(400, 100));

	/////////// --- BACKGROUND LOBBY --- //////////
	sf::Texture textureLobby;
	if (!textureLobby.loadFromFile("backgroundLobby.jpg"))
	{
		std::cout << "No se ha encontrado el fondo de pantalla de la Lobby" << std::endl;
	}
	sf::Sprite spriteLobby;
	sf::Vector2u size = textureLobby.getSize();
	spriteLobby.setTexture(textureLobby);

	//DRAW
	window.draw(spriteLobby);
	window.draw(rectangleLobby);
	window.draw(lobbyText);
	window.draw(editTextTitle);
	window.display();

	//RECOGER ALIAS
	std::cout << "What's your name: ";
	std::cin >> aliasName;

	//CONSTRUIMOS EL JUGADOR
	PlayerInfo playerInfo;
	playerInfo.name = aliasName;

	//ENVIAMOS INFO DEL JUGADOR
	pack << Ordenes::CreatePlayer;
	pack << playerInfo;
	sock.send(pack);

	//ESPERANDO A QUE SE CONECTEN 4 JUGADORES


	//BUCLE DE JUEGO
	while (running)
	{
		while (window.isOpen())
		{
			sf::Event evento;
			while (window.pollEvent(evento))
			{
				switch (evento.type)
				{
				case sf::Event::Closed:


					break;
				case sf::Event::KeyPressed:
					if (evento.key.code == sf::Keyboard::Escape)

						window.close();
					else if (evento.key.code == sf::Keyboard::Return)
					{
						
					}
					break;
				}
			}

			//PANTALLA DE JUEGO
			window.display();
			window.clear();
		}
	}

	return 0;
}