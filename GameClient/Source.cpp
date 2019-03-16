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
#define PORT 50001
#define IP "127.0.0.1"

//VARIABLES GLOBALES
bool running = true;
bool startGame = false;
sf::Socket::Status status;
sf::Mutex mtx;

//VECTOR PLAYER
std::vector<PlayerInfo> players;
int VectorPlayersSize;
int indexPlayer;

//Vector2 mouse
sf::Vector2<int> mousePos;

//ENUM (ORDENES DEL CLIENTE)
enum Ordenes
{
	CreatePlayer,
	ThrowDices
};

//OVERCHARGED FUNCTIONS (PLAYER INFO)
sf::Packet& operator <<(sf::Packet& packet, const PlayerInfo& playerInfo)
{
	return packet << playerInfo.name << playerInfo.position.x << playerInfo.position.y << playerInfo.money << playerInfo.isYourTurn;
}
//OVERCHARGED FUNCTIONS (ENUM CLASS)
sf::Packet& operator <<(sf::Packet& packet, const Ordenes& orders)
{
	int option = static_cast<int>(orders);
	std::cout << option << std::endl;
	return packet << option;
}

//MENSAJES DEL SERVIDOR
void RecepcionMensaje(sf::TcpSocket* sock)
{
	sf::Packet packReceive;
	int order;

	//VARIABLES SIMPLES
	PlayerInfo auxPlayer;
	std::string auxName;
	int auxPositionX;
	int auxPositionY;
	int auxMoney;
	bool auxIsYourTurn;
	int auxId;
	//RECIBIMOS LA INFORMACION DE TODOS LOS JUGADORES
	status=sock->receive(packReceive);
	if (status == sf::Socket::Done) {
		//RECOGEMOS TODA LA INFO DE LOS PLAYERS
		packReceive >> VectorPlayersSize;
		//Rellenamos vector
		if (VectorPlayersSize == 4) {
			for (int i = 0; i < VectorPlayersSize; i++)
			{
				packReceive >> auxId;
				packReceive >> auxName;
				packReceive >> auxPositionX;
				packReceive >> auxPositionY;
				packReceive >> auxMoney;
				packReceive >> auxIsYourTurn;

				//RELLENAMOS VECTOR DE PLAYERS
				auxPlayer.id = auxId;
				auxPlayer.name = auxName;
				auxPlayer.position.x = auxPositionX;
				auxPlayer.position.y = auxPositionY;
				auxPlayer.money = auxMoney;
				auxPlayer.isYourTurn = auxIsYourTurn;
				players.push_back(auxPlayer);

			}
		}
		startGame = true;
	}

	while (running)
	{
		for (int i = 0; i < VectorPlayersSize; i++)
		{
			std::cout << players[i].id << std::endl;
			std::cout << players[i].name << std::endl;
			std::cout << players[i].position.x << std::endl;
			std::cout << players[i].position.y << std::endl;
			std::cout << players[i].money << std::endl;
			std::cout << players[i].isYourTurn << std::endl;
		}

		status = sock->receive(packReceive);
		if (status == sf::Socket::Done)
		{
			packReceive >> order;
			switch (order)
			{
			case 0:
				break;
			case 1:
				break;
			case 2:
				break;
			}
		}
	}
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
	sf::Text lobbyText("Monopoly Game", fontCourbd, 34);
	lobbyText.setFillColor(sf::Color(255, 255, 255));
	lobbyText.setPosition(500 - 140, 400 - 150);
	lobbyText.setStyle(sf::Text::Bold);

	//--- PUTTEXT TITLE ---//
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

	////////////// --- BUTTONMYTURN --- ////////////
	sf::RectangleShape buttonMyTurn(sf::Vector2f(100, 50));
	buttonMyTurn.setFillColor(sf::Color(255, 165, 0, 255));
	buttonMyTurn.setOutlineThickness(5);
	buttonMyTurn.setOutlineColor(sf::Color(255, 165, 0, 255));
	buttonMyTurn.setPosition(900, 750);
	//--- Text Throw Dices ---//
	sf::Text TextThrowDiecs("Throw Dices", fontCourbd, 10);
	TextThrowDiecs.setFillColor(sf::Color(0, 0, 0));
	TextThrowDiecs.setPosition(910, 765);
	TextThrowDiecs.setStyle(sf::Text::Bold);


	////////////// --- BUTTONNOTMYTURN --- ////////////
	sf::RectangleShape buttonNotMyTurn(sf::Vector2f(100, 50));
	buttonNotMyTurn.setFillColor(sf::Color(120, 120, 120, 255));
	buttonNotMyTurn.setOutlineThickness(5);
	buttonNotMyTurn.setOutlineColor(sf::Color(120, 120, 120, 255));
	buttonNotMyTurn.setPosition(900, 750);

	/////////// --- BACKGROUND LOBBY --- //////////
	sf::Texture textureLobby;
	if (!textureLobby.loadFromFile("backgroundLobby.jpg"))
	{
		std::cout << "No se ha encontrado el fondo de pantalla de la Lobby" << std::endl;
	}
	sf::Sprite spriteLobby;
	//sf::Vector2u size = textureLobby.getSize();
	spriteLobby.setScale(sf::Vector2f(2.0f, 2.0f));
	spriteLobby.setTexture(textureLobby);

	//DRAW
	window.draw(spriteLobby);
	window.draw(rectangleLobby);
	window.draw(lobbyText);
	window.draw(editTextTitle);
	window.display();
	window.clear();

	//RECOGER ALIAS
	std::cout << "What's your name: ";
	std::cin >> aliasName;

	//ENVIAMOS INFO DEL JUGADOR
	pack << Ordenes::CreatePlayer;
	pack << aliasName;
	status=sock.send(pack);
	if (status == sf::Socket::Done)
	{
		std::cout<<"He enviado mi info al servidor"<<std::endl;
	}
	//ESPERANDO A QUE SE CONECTEN 4 JUGADORES
	while (!startGame)
	{
		sf::Text waitingText("Waiting other players ... ", fontCourbd, 34);
		waitingText.setFillColor(sf::Color(255, 255, 255));
		waitingText.setPosition(500 - 200, 400 - 150);
		waitingText.setStyle(sf::Text::Bold);

		window.draw(waitingText);

		window.display();
		window.clear();
	}

	//////// --- TABLERO DEL JUEGO --- ////////////
	sf::Texture textureBoard;
	if (!textureBoard.loadFromFile("chess.jpg"))
	{
		std::cout << "No se ha encontrado el fondo de pantalla de la chess" << std::endl;
	}
	sf::Sprite spriteBoard;
	//sf::Vector2u size = textureLobby.getSize();
	spriteBoard.setScale(sf::Vector2f(0.5f, 0.5f));
	spriteBoard.setTexture(textureBoard);

	//ID PROPIO DEL JUGADOR
	for (int i = 0; i < players.size(); i++)
	{
		if (players[i].name == aliasName)
		{
			//LA POSICION DEL JUGADOR DENTRO DEL VECTOR
			indexPlayer = i;
		}
	}

	//TEXTO Alias Player
	sf::Text myPlayer;
	sf::Text Player1Text;
	sf::Text Player2Text;
	sf::Text Player3Text;

	//TEXTO Dinero Player
	sf::Text myPlayerMoney;
	sf::Text Player1Money;
	sf::Text Player2Money;
	sf::Text Player3Money;

	//CIRCLE TOKEN
	sf::CircleShape myPlayerToken(10);
	sf::CircleShape Player1Token(10);
	sf::CircleShape Player2Token(10);
	sf::CircleShape Player3Token(10);

	int counterPlayerText = 0;
	std::string CorrectMoney;
	for (int i = 0; i < players.size(); i++)
	{
		//Printamos el texto
		if (players[i].name == aliasName)
		{
			//TEXT
			myPlayer = { players[i].name, fontCourbd, 24 };
			myPlayer.setFillColor(sf::Color(255, 0, 0));
			myPlayer.setPosition(50, 650);
			myPlayer.setStyle(sf::Text::Bold);

			//MONEY
			CorrectMoney = std::to_string(players[i].money).append("$");
			myPlayerMoney = { CorrectMoney, fontCourbd, 24 };
			myPlayerMoney.setFillColor(sf::Color(52, 73, 40));
			myPlayerMoney.setPosition(50, 680);
			myPlayerMoney.setStyle(sf::Text::Bold);

			//CIRCLE TOKEN
			myPlayerToken.setPosition(players[i].position.x, players[i].position.y);
			myPlayerToken.setFillColor(sf::Color(255, 0, 0));
			myPlayerToken.setOutlineThickness(2);
			myPlayerToken.setOutlineColor(sf::Color(0, 0, 0));
		}
		else
		{
			switch (counterPlayerText)
			{
			case 0:
				//TEXT
				Player1Text = { players[i].name, fontCourbd, 24 };
				Player1Text.setFillColor(sf::Color(0, 255, 0));
				Player1Text.setPosition(700, 100);
				Player1Text.setStyle(sf::Text::Bold);

				//MONEY
				CorrectMoney = std::to_string(players[i].money).append("$");
				Player1Money = {CorrectMoney, fontCourbd, 24 };
				Player1Money.setFillColor(sf::Color(52, 73, 40));
				Player1Money.setPosition(700, 130);
				Player1Money.setStyle(sf::Text::Bold);

				//CIRCLE TOKEN
				Player1Token.setPosition(players[i].position.x, players[i].position.y);
				Player1Token.setFillColor(sf::Color(0, 255, 0));
				Player1Token.setOutlineThickness(2);
				Player1Token.setOutlineColor(sf::Color(0, 0, 0));
				break;
			case 1:
				//TEXT
				Player2Text = { players[i].name, fontCourbd, 24 };
				Player2Text.setFillColor(sf::Color(0, 0, 255));
				Player2Text.setPosition(700, 300);
				Player2Text.setStyle(sf::Text::Bold);

				//MONEY
				CorrectMoney = std::to_string(players[i].money).append("$");
				Player2Money = { CorrectMoney, fontCourbd, 24 };
				Player2Money.setFillColor(sf::Color(52, 73, 40));
				Player2Money.setPosition(700, 330);
				Player2Money.setStyle(sf::Text::Bold);

				//CIRCLE TOKEN
				Player2Token.setPosition(players[i].position.x, players[i].position.y);
				Player2Token.setFillColor(sf::Color(0, 0, 255));
				Player2Token.setOutlineThickness(2);
				Player2Token.setOutlineColor(sf::Color(0, 0, 0));
				break;
			case 2:
				//TEXT
				Player3Text = { players[i].name, fontCourbd, 24 };
				Player3Text.setFillColor(sf::Color(128, 128, 255));
				Player3Text.setPosition(700, 500);
				Player3Text.setStyle(sf::Text::Bold);

				//MONEY
				CorrectMoney = std::to_string(players[i].money).append("$");
				Player3Money = { CorrectMoney, fontCourbd, 24 };
				Player3Money.setFillColor(sf::Color(52, 73, 40));
				Player3Money.setPosition(700, 530);
				Player3Money.setStyle(sf::Text::Bold);

				//CIRCLE TOKEN
				Player3Token.setPosition(players[i].position.x, players[i].position.y);
				Player3Token.setFillColor(sf::Color(128, 128, 255));
				Player3Token.setOutlineThickness(2);
				Player3Token.setOutlineColor(sf::Color(0, 0, 0));
				break;
			}
			counterPlayerText++;
		}
	}

	//BUCLE DE JUEGO
	while (window.isOpen() && running)
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
				case sf::Event::MouseButtonPressed:
					if (evento.mouseButton.button == sf::Mouse::Left)
					{
						if (players[indexPlayer].isYourTurn)
						{
							std::cout << "the right button was pressed" << std::endl;
							std::cout << "mouse x: " << evento.mouseButton.x << std::endl;
							std::cout << "mouse y: " << evento.mouseButton.y << std::endl;
							//Preparamos pack
							pack.clear();
							pack << Ordenes::ThrowDices;		
							pack << indexPlayer;
							status = sock.send(pack);
							if (status == sf::Socket::Done)
							{
								//std::cout << indexPlayer << std::endl;
							}
						}
					}
					break;

			}
		}

		//CONTROL MOUSE
		/*if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
		{
			mousePos = sf::Mouse::getPosition();
			if ((mousePos.x>800)&&(mousePos.y > 600))
			{
				std::cout << "LLego" << std::endl;
			}
		}*/
		//DRAW 
		//TEXT 
		window.draw(myPlayer);
		window.draw(Player1Text);
		window.draw(Player2Text);
		window.draw(Player3Text);
		
		//MONEY
		window.draw(myPlayerMoney);
		window.draw(Player1Money);
		window.draw(Player2Money);
		window.draw(Player3Money);

		//BOARD
		window.draw(spriteBoard);

		//TOKENs
		window.draw(myPlayerToken);
		window.draw(Player1Token);
		window.draw(Player2Token);
		window.draw(Player3Token);

		//BUTTON
		//SI E TU TURNO
		if (players[indexPlayer].isYourTurn)
		{
			window.draw(buttonMyTurn);
			
		}
		else 
		{
			window.draw(buttonNotMyTurn);
		}
		//TextThrowDices
		window.draw(TextThrowDiecs);

		//PANTALLA DE JUEGO
		window.display();
		window.clear();
	
	}

	return 0;
}