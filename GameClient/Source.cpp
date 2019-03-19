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
#include <stdio.h>      
#include <stdlib.h>     
#include <time.h>      

//CONSTANTES
#define PORT 50001
//#define PORT 50000
#define IP "127.0.0.1"

//ENUM (ORDENES DEL CLIENTE)
enum Ordenes
{
	CreatePlayer,
	ThrowDices,
	DecideBuy,
	DecidePayJail
};

enum Acciones
{
	NONE,
	PUEDOCOMPRAR,
	SALIRDELACARCEL
};

//VARIABLES GLOBALES
bool running = true;
bool startGame = false;
sf::Socket::Status status;
sf::Mutex mtx;
Acciones accion = Acciones::NONE;
int owner=-1;
int counteCasillasUsadas = 0;

//struct color asociado
struct ColorPlayer
{
	int indexPlayer;
	int color;
};

//VECTOR PLAYER
std::vector<PlayerInfo> players;
int VectorPlayersSize;
int indexPlayer;
bool HasBuyed = false;
//COLOR ASOCIADO AL PLAYER
ColorPlayer JugadorColor[4];

//TEXTO Dinero Player
sf::Text playerMoney[4];

//Aux buy
sf::Text propiertyToBuy;

//CIRCLE TOKEN
sf::CircleShape playerTokens[4];

//Vector2 mouse
sf::Vector2<int> mousePos;

//TextForJail
sf::Text turnsInJail;

//RectanglesBuyed
sf::RectangleShape Buyed[40];

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
	std::string auxJail;
	int auxPositionX;
	int auxPositionY;
	int auxMoney;
	int auxMoney2;
	bool auxIsYourTurn;
	int auxId;
	int auxCasilla;
	int auxTipo = -1;
	int auxPrecioPropiedad;
	int auxNewTurn;
	int auxIdOwner;
	int auxNumberTurnsToLose = 0;

	//RECIBIMOS LA INFORMACION DE TODOS LOS JUGADORES
	status=sock->receive(packReceive);
	if (status == sf::Socket::Done) 
	{
		//RECOGEMOS TODA LA INFO DE LOS PLAYERS
		packReceive >> VectorPlayersSize;
		//Rellenamos vector
		if (VectorPlayersSize == 4) 
		{
			for (int i = 0; i < VectorPlayersSize; i++)
			{
				packReceive >> auxId;
				packReceive >> auxName;
				packReceive >> auxPositionX;
				packReceive >> auxPositionY;
				packReceive >> auxMoney;
				packReceive >> auxIsYourTurn;
				packReceive >> auxCasilla;


				//RELLENAMOS VECTOR DE PLAYERS
				auxPlayer.id = auxId;
				auxPlayer.name = auxName;
				auxPlayer.position.x = auxPositionX;
				auxPlayer.position.y = auxPositionY;
				auxPlayer.money = auxMoney;
				auxPlayer.isYourTurn = auxIsYourTurn;
				auxPlayer.casilla = auxCasilla;
				players.push_back(auxPlayer);

			}
		}
		startGame = true;
	}
	//JUEGO//////////////////////////////////////////////////////////////////
	packReceive.clear();
	while (running)
	{
		/*for (int i = 0; i < VectorPlayersSize; i++)
		{
			std::cout << players[i].id << std::endl;
			std::cout << players[i].name << std::endl;
			std::cout << players[i].position.x << std::endl;
			std::cout << players[i].position.y << std::endl;
			std::cout << players[i].money << std::endl;
			std::cout << players[i].isYourTurn << std::endl;
		}*/

		status = sock->receive(packReceive);
		if (status == sf::Socket::Done)
		{
			packReceive >> order;
			switch (order)
			{
			case 0:
				break;
			case 1://ResultDices
				packReceive >> auxId;
				packReceive >> auxCasilla;
				packReceive >> auxPositionX;
				packReceive >> auxPositionY;		
				packReceive >> auxTipo;								
				switch (auxTipo)
				{
				case 0://Propiedad
					packReceive >> owner;
					if (owner == -1)
					{
						packReceive >> auxPrecioPropiedad;
						//pregunto si quiero comprar
						std::cout << auxPrecioPropiedad << std::endl;
						propiertyToBuy.setString(std::to_string(auxPrecioPropiedad));
						if (players[indexPlayer].isYourTurn)
						{
							accion = Acciones::PUEDOCOMPRAR;
						}
					}
					else 
					{
						packReceive >> auxMoney;
						packReceive >> auxMoney2;
						packReceive >> auxIdOwner;
						packReceive >> auxId;
						packReceive >> auxNewTurn;
						//Update
						players[auxId].money = auxMoney;
						players[auxIdOwner].money = auxMoney2;
						players[auxId].isYourTurn = false;
						players[auxNewTurn].isYourTurn = true;
						std::cout << "New turn :" << auxNewTurn << std::endl;
						//Actualizo mi dinero y el del propietario
						playerMoney[auxId].setString(std::to_string(players[auxId].money).append("$"));
						playerMoney[auxIdOwner].setString(std::to_string(players[auxIdOwner].money).append("$"));
					}
					break;
				case 1://Estacion
					packReceive >> owner;
					if (owner == -1)
					{
						packReceive >> auxPrecioPropiedad;
						//pregunto si quiero comprar
						std::cout << auxPrecioPropiedad << std::endl;
						propiertyToBuy.setString(std::to_string(auxPrecioPropiedad));
						if (players[indexPlayer].isYourTurn) 
						{
							accion = Acciones::PUEDOCOMPRAR;
						}
					}
					else
					{
						packReceive >> auxMoney;
						packReceive >> auxMoney2;
						packReceive >> auxIdOwner;
						packReceive >> auxId;
						packReceive >> auxNewTurn;
						//Update
						players[auxId].money = auxMoney;
						players[auxIdOwner].money = auxMoney2;
						players[auxId].isYourTurn = false;
						players[auxNewTurn].isYourTurn = true;
						std::cout << "New turn :" << auxNewTurn << std::endl;
						//Actualizo mi dinero y el del propietario
						playerMoney[auxId].setString(std::to_string(players[auxId].money).append("$") );
						playerMoney[auxIdOwner].setString(std::to_string(players[auxIdOwner].money).append("$"));						
					}
					break;
				case 2://Neutra
					packReceive >> auxId;
					packReceive >> auxNewTurn;
					//UPDATE MONEY					
					players[auxId].isYourTurn = false;
					players[auxNewTurn].isYourTurn = true;
					break;
				case 3://FreeMoney
					packReceive >> auxMoney;
					packReceive >> auxId;
					packReceive >> auxNewTurn;
					//UPDATE MONEY
					players[auxId].money = auxMoney;
					players[auxId].isYourTurn = false;
					players[auxNewTurn].isYourTurn = true;

					playerMoney[auxId].setString(std::to_string(players[auxId].money).append("$"));
					break;
				case 4://Tax
					packReceive >> auxMoney;
					packReceive >> auxId;
					packReceive >> auxNewTurn;
					//UPDATE MONEY
					players[auxId].money = auxMoney;
					players[auxId].isYourTurn = false;
					players[auxNewTurn].isYourTurn = true;

					playerMoney[auxId].setString(std::to_string(players[auxId].money).append("$"));
					break;
				case 5://Jail
					packReceive >> auxCasilla;
					packReceive >> auxPositionX;
					packReceive >> auxPositionY;
					packReceive >> auxId;
					packReceive >> auxNewTurn;
					//Update
					players[auxId].isYourTurn = false;
					players[auxNewTurn].isYourTurn = true;
					if (indexPlayer==auxId)
					{
						accion = Acciones::SALIRDELACARCEL;
					}
					break;
				case 6://Company	
					packReceive >> owner;
					if (owner == -1)
					{
						packReceive >> auxPrecioPropiedad;
						//pregunto si quiero comprar
						std::cout << auxPrecioPropiedad << std::endl;
						propiertyToBuy.setString(std::to_string(auxPrecioPropiedad));
						if (players[indexPlayer].isYourTurn)
						{
							accion = Acciones::PUEDOCOMPRAR;
						}
					}
					else
					{
						packReceive >> auxMoney;
						packReceive >> auxMoney2;
						packReceive >> auxIdOwner;
						packReceive >> auxId;
						packReceive >> auxNewTurn;
						//Update
						players[auxId].money = auxMoney;
						players[auxIdOwner].money = auxMoney2;
						players[auxId].isYourTurn = false;
						players[auxNewTurn].isYourTurn = true;
						std::cout << "New turn :" << auxNewTurn << std::endl;
						//Actualizo mi dinero y el del propietario
						playerMoney[auxId].setString(std::to_string(players[auxId].money).append("$"));
						playerMoney[auxIdOwner].setString(std::to_string(players[auxIdOwner].money).append("$"));
					}
					break;
				default:
					break;
				}					
				
				//Actualizamos Player que ha tirado//////
				players[auxId].casilla = auxCasilla;
				players[auxId].position.x = auxPositionX;
				players[auxId].position.y = auxPositionY;
				playerTokens[auxId].setPosition(players[auxId].position.x, players[auxId].position.y);				
				break;
			case 2://Actualizar dinero
				packReceive >> auxMoney;
				packReceive >> auxId;
				packReceive >> auxNewTurn;				
				//Update dinero
				players[auxId].money = auxMoney;
				players[auxId].isYourTurn = false;
				players[auxNewTurn].isYourTurn = true;
				//Update text
				playerMoney[auxId].setString(std::to_string(players[auxId].money).append("$"));

				//MARCAR DE QUIEN ES LA CASILLA
				switch (auxId)
				{
				case 0:
					Buyed[counteCasillasUsadas].setSize(sf::Vector2f(25, 25));
					Buyed[counteCasillasUsadas].setOutlineThickness(5);
					Buyed[counteCasillasUsadas].setOutlineColor(sf::Color(0, 0, 0, 255));
					for (int i = 0; i < 4; i++)
					{
						if (auxId == JugadorColor[i].indexPlayer)
						{
							switch (JugadorColor[i].color)
							{
							case 0:
								Buyed[counteCasillasUsadas].setFillColor(sf::Color(255, 0, 0));
								break;
							case 1:
								Buyed[counteCasillasUsadas].setFillColor(sf::Color(0, 255, 0));
								break;
							case 2:
								Buyed[counteCasillasUsadas].setFillColor(sf::Color(0, 0, 255));
								break;
							case 3:
								Buyed[counteCasillasUsadas].setFillColor(sf::Color(128, 128, 255));
								break;
							default:
								break;
							}
						}
					}
					break;
				case 1:
					Buyed[counteCasillasUsadas].setSize(sf::Vector2f(25, 25));
					Buyed[counteCasillasUsadas].setOutlineThickness(5);
					Buyed[counteCasillasUsadas].setOutlineColor(sf::Color(0, 0, 0, 255));
					for (int i = 0; i < 4; i++)
					{
						if (auxId == JugadorColor[i].indexPlayer)
						{
							switch (JugadorColor[i].color)
							{
							case 0:
								Buyed[counteCasillasUsadas].setFillColor(sf::Color(255, 0, 0));
								break;
							case 1:
								Buyed[counteCasillasUsadas].setFillColor(sf::Color(0, 255, 0));
								break;
							case 2:
								Buyed[counteCasillasUsadas].setFillColor(sf::Color(0, 0, 255));
								break;
							case 3:
								Buyed[counteCasillasUsadas].setFillColor(sf::Color(128, 128, 255));
								break;
							default:
								break;
							}
						}
					}
					break;
				case 2:
					Buyed[counteCasillasUsadas].setSize(sf::Vector2f(25, 25));
					Buyed[counteCasillasUsadas].setOutlineThickness(5);
					Buyed[counteCasillasUsadas].setOutlineColor(sf::Color(0, 0, 0, 255));
					for (int i = 0; i < 4; i++)
					{
						if (auxId == JugadorColor[i].indexPlayer)
						{
							switch (JugadorColor[i].color)
							{
							case 0:
								Buyed[counteCasillasUsadas].setFillColor(sf::Color(255, 0, 0));
								break;
							case 1:
								Buyed[counteCasillasUsadas].setFillColor(sf::Color(0, 255, 0));
								break;
							case 2:
								Buyed[counteCasillasUsadas].setFillColor(sf::Color(0, 0, 255));
								break;
							case 3:
								Buyed[counteCasillasUsadas].setFillColor(sf::Color(128, 128, 255));
								break;
							default:
								break;
							}
						}
					}
					break;
				case 3:
					Buyed[counteCasillasUsadas].setSize(sf::Vector2f(25, 25));
					Buyed[counteCasillasUsadas].setOutlineThickness(5);
					Buyed[counteCasillasUsadas].setOutlineColor(sf::Color(0, 0, 0, 255));
					for (int i = 0; i < 4; i++)
					{
						if (auxId == JugadorColor[i].indexPlayer)
						{
							switch (JugadorColor[i].color)
							{
							case 0:
								Buyed[counteCasillasUsadas].setFillColor(sf::Color(255, 0, 0));
								break;
							case 1:
								Buyed[counteCasillasUsadas].setFillColor(sf::Color(0, 255, 0));
								break;
							case 2:
								Buyed[counteCasillasUsadas].setFillColor(sf::Color(0, 0, 255));
								break;
							case 3:
								Buyed[counteCasillasUsadas].setFillColor(sf::Color(128, 128, 255));
								break;
							default:
								break;
							}
						}
					}
					break;
				default:
					break;
				}
				//POSICION
				Buyed[counteCasillasUsadas].setPosition(players[auxId].position.x, players[auxId].position.y - 60);
				counteCasillasUsadas++;
				break;
			case 3://NewTurn
				packReceive >> auxNumberTurnsToLose;
				packReceive >> auxId;
				packReceive >> auxNewTurn;
				players[auxId].isYourTurn = false;
				players[auxNewTurn].isYourTurn = true;	
				std::cout << "Turnos restantes :" << auxNumberTurnsToLose << std::endl;
				std::cout << "AuxId :" << auxId << std::endl;
				std::cout << "indexPlayer :" << indexPlayer << std::endl;
				if (auxId == indexPlayer)
				{
					if (auxNumberTurnsToLose == 0)
					{
						accion = Acciones::NONE;
					}
				}
				break;
			case 4://Charge
				
				break;
			case 5://AcutalizeMoneyRound
				packReceive >> auxMoney;
				packReceive >> auxId;
				players[auxId].money = auxMoney;
				playerMoney[auxId].setString(std::to_string(players[auxId].money));
				if (auxId==indexPlayer)
				{
					accion = Acciones::NONE;
				}
				break;			
			}
		}
		//Limpiamos pack
		packReceive.clear();
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

	////////////// --- BUTTON MY TURN --- ////////////
	sf::RectangleShape buttonMyTurn(sf::Vector2f(100, 50));
	buttonMyTurn.setFillColor(sf::Color(255, 165, 0, 255));
	buttonMyTurn.setOutlineThickness(5);
	buttonMyTurn.setOutlineColor(sf::Color(255, 165, 0, 255));
	buttonMyTurn.setPosition(900, 750);
	//--- Text Throw Dices ---//
	sf::Text TextThrowDices("Throw Dices", fontCourbd, 10);
	TextThrowDices.setFillColor(sf::Color(0, 0, 0));
	TextThrowDices.setPosition(910, 765);
	TextThrowDices.setStyle(sf::Text::Bold);

	//--- Text I wnt to Buy---//
	sf::Text TextBuy("Buy!", fontCourbd, 14);
	TextBuy.setFillColor(sf::Color(0, 0, 0));
	TextBuy.setPosition(910, 765);
	TextBuy.setStyle(sf::Text::Bold);


	//TEXT DISPLAY PRICE AND NAMEPROPIERTY///////////////propiertyToBuy
	//text price
	propiertyToBuy = { "Price!", fontCourbd, 40 };
	propiertyToBuy.setFillColor(sf::Color(0, 0, 0));
	propiertyToBuy.setPosition(280, 300);
	propiertyToBuy.setStyle(sf::Text::Bold);
	//rectangle 
	sf::RectangleShape rectPrice(sf::Vector2f(100, 50));
	rectPrice.setFillColor(sf::Color(255, 165, 0, 255));
	rectPrice.setOutlineThickness(5);
	rectPrice.setOutlineColor(sf::Color(255, 165, 0, 255));
	rectPrice.setPosition(270, 295);

	//Text for jail
	turnsInJail = { "Jail!", fontCourbd, 14 };
	turnsInJail.setFillColor(sf::Color(0, 0, 0));
	turnsInJail.setPosition(280, 300);
	turnsInJail.setStyle(sf::Text::Bold);
	//PriceJail
	sf::Text JailPrice("You Have to pay 50$", fontCourbd, 14);
	JailPrice.setFillColor(sf::Color(0, 0, 0));
	JailPrice.setPosition(270, 310);
	JailPrice.setStyle(sf::Text::Bold);
	//rectangle Jail
	sf::RectangleShape rectJail(sf::Vector2f(200, 50));
	rectJail.setFillColor(sf::Color(255, 165, 0, 255));
	rectJail.setOutlineThickness(5);
	rectJail.setOutlineColor(sf::Color(255, 165, 0, 255));
	rectJail.setPosition(265, 290);

	//text ButtonNotBuy
	sf::Text TextNotBuy("Not Buy", fontCourbd, 14);
	TextNotBuy.setFillColor(sf::Color(0, 0, 0));
	TextNotBuy.setPosition(5, 765);
	TextNotBuy.setStyle(sf::Text::Bold);
	//RectangleNotBuy
	sf::RectangleShape ButtonNotBuy(sf::Vector2f(100, 50));
	ButtonNotBuy.setFillColor(sf::Color(255, 165, 0, 255));
	ButtonNotBuy.setOutlineThickness(5);
	ButtonNotBuy.setOutlineColor(sf::Color(255, 165, 0, 255));
	ButtonNotBuy.setPosition(0, 750);

	////////////// --- BUTTON NOT MY TURN --- ////////////
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

	int counterPlayerText = 0;
	std::string CorrectMoney;
	for (int i = 0; i < players.size(); i++)
	{
		playerTokens[i].setRadius(10);
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
			playerMoney[i] = { CorrectMoney, fontCourbd, 24 };
			playerMoney[i].setFillColor(sf::Color(52, 73, 40));
			playerMoney[i].setPosition(50, 680);
			playerMoney[i].setStyle(sf::Text::Bold);

			//CIRCLE TOKEN
			playerTokens[i].setPosition(players[i].position.x, players[i].position.y);
			playerTokens[i].setFillColor(sf::Color(255, 0, 0));
			playerTokens[i].setOutlineThickness(2);
			playerTokens[i].setOutlineColor(sf::Color(0, 0, 0));

			//JUGADOR COLOR ASOCIADO
			JugadorColor[i].indexPlayer = i;
			JugadorColor[i].color = 0;
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
				playerMoney[i] = {CorrectMoney, fontCourbd, 24 };
				playerMoney[i].setFillColor(sf::Color(52, 73, 40));
				playerMoney[i].setPosition(700, 130);
				playerMoney[i].setStyle(sf::Text::Bold);

				//CIRCLE TOKEN
				playerTokens[i].setPosition(players[i].position.x, players[i].position.y);
				playerTokens[i].setFillColor(sf::Color(0, 255, 0));
				playerTokens[i].setOutlineThickness(2);
				playerTokens[i].setOutlineColor(sf::Color(0, 0, 0));

				//JUGADOR COLOR ASOCIADO
				JugadorColor[i].indexPlayer = i;
				JugadorColor[i].color = 1;
				break;
			case 1:
				//TEXT
				Player2Text = { players[i].name, fontCourbd, 24 };
				Player2Text.setFillColor(sf::Color(0, 0, 255));
				Player2Text.setPosition(700, 300);
				Player2Text.setStyle(sf::Text::Bold);

				//MONEY
				CorrectMoney = std::to_string(players[i].money).append("$");
				playerMoney[i] = { CorrectMoney, fontCourbd, 24 };
				playerMoney[i].setFillColor(sf::Color(52, 73, 40));
				playerMoney[i].setPosition(700, 330);
				playerMoney[i].setStyle(sf::Text::Bold);

				//CIRCLE TOKEN
				playerTokens[i].setPosition(players[i].position.x, players[i].position.y);
				playerTokens[i].setFillColor(sf::Color(0, 0, 255));
				playerTokens[i].setOutlineThickness(2);
				playerTokens[i].setOutlineColor(sf::Color(0, 0, 0));

				//JUGADOR COLOR ASOCIADO
				JugadorColor[i].indexPlayer = i;
				JugadorColor[i].color = 2;
				break;
			case 2:
				//TEXT
				Player3Text = { players[i].name, fontCourbd, 24 };
				Player3Text.setFillColor(sf::Color(128, 128, 255));
				Player3Text.setPosition(700, 500);
				Player3Text.setStyle(sf::Text::Bold);

				//MONEY
				CorrectMoney = std::to_string(players[i].money).append("$");
				playerMoney[i] = { CorrectMoney, fontCourbd, 24 };
				playerMoney[i].setFillColor(sf::Color(52, 73, 40));
				playerMoney[i].setPosition(700, 530);
				playerMoney[i].setStyle(sf::Text::Bold);

				//CIRCLE TOKEN
				playerTokens[i].setPosition(players[i].position.x, players[i].position.y);
				playerTokens[i].setFillColor(sf::Color(128, 128, 255));
				playerTokens[i].setOutlineThickness(2);
				playerTokens[i].setOutlineColor(sf::Color(0, 0, 0));

				//JUGADOR COLOR ASOCIADO
				JugadorColor[i].indexPlayer = i;
				JugadorColor[i].color = 3;
				break;
			}
			counterPlayerText++;
		}
	}
	
	//STRUCT
	/*for (int i = 0; i < 4; i++)
	{
		std::cout << "Index: " << JugadorColor[i].indexPlayer << std::endl;
		std::cout << "Color: " << JugadorColor[i].color << std::endl;
	}*/


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
						/*std::cout << "the right button was pressed" << std::endl;
						std::cout << "mouse x: " << evento.mouseButton.x << std::endl;
						std::cout << "mouse y: " << evento.mouseButton.y << std::endl;*/
						if ((players[indexPlayer].isYourTurn==true) && (accion==Acciones::NONE))
						{							
							
							if ((evento.mouseButton.x > 900) && (evento.mouseButton.y > 750))
							{
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
						else if ((players[indexPlayer].isYourTurn == true) && (accion == Acciones::PUEDOCOMPRAR))
						{
							if ((evento.mouseButton.x < 100) && (evento.mouseButton.y > 750))//NoCompramos
							{
								//Preparamos pack
								pack.clear();
								pack << Ordenes::DecideBuy;
								pack << indexPlayer;
								pack << false;
								status = sock.send(pack);
								if (status == sf::Socket::Done)
								{
									std::cout << "Player send " << indexPlayer << std::endl;
								}
								accion = Acciones::NONE;
							}
							else if ((evento.mouseButton.x > 900) && (evento.mouseButton.y > 750))//Compramos
							{
								pack.clear();
								pack << Ordenes::DecideBuy;
								pack << indexPlayer;
								pack << true;
								status = sock.send(pack);
								if (status == sf::Socket::Done)
								{
									std::cout <<"Player send " << indexPlayer << std::endl;
								}
								accion = Acciones::NONE;
							}
						}
						else if ((players[indexPlayer].isYourTurn == true) && (accion == Acciones::SALIRDELACARCEL))
						{
							if ((evento.mouseButton.x < 100) && (evento.mouseButton.y > 750))//NoCompramos
							{
								//Preparamos pack
								pack.clear();
								pack << Ordenes::DecidePayJail;
								pack << indexPlayer;
								pack << false;
								status = sock.send(pack);
								std::cout << "Player id: " << indexPlayer << std::endl;
								if (status == sf::Socket::Done)
								{
									std::cout << "Player send " << indexPlayer << std::endl;
								}
							}
							else if ((evento.mouseButton.x > 900) && (evento.mouseButton.y > 750))//Compramos
							{
								pack.clear();
								pack << Ordenes::DecidePayJail;
								pack << indexPlayer;
								pack << true;
								status = sock.send(pack);
								if (status == sf::Socket::Done)
								{
									std::cout << "Player send " << indexPlayer << std::endl;
								}
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
		window.draw(playerMoney[0]);
		window.draw(playerMoney[1]);
		window.draw(playerMoney[2]);
		window.draw(playerMoney[3]);

		//BOARD
		window.draw(spriteBoard);

		//TOKENs
		window.draw(playerTokens[0]);
		window.draw(playerTokens[1]);
		window.draw(playerTokens[2]);
		window.draw(playerTokens[3]);

		//COSAS COMPRADAS
		for (int i = 0; i < 40; i++)
		{
			window.draw(Buyed[i]);
		}

		//BUTTON
		//SI E TU TURNO
		if (players[indexPlayer].isYourTurn)
		{
			window.draw(buttonMyTurn);
			if (accion == Acciones::NONE)
			{
				window.draw(TextThrowDices);
			}
			else if (accion == Acciones::PUEDOCOMPRAR)
			{
				//Buy text
				window.draw(TextBuy);
				//Price
				window.draw(rectPrice);
				window.draw(propiertyToBuy);
				//Button not to buy
				window.draw(ButtonNotBuy);
				window.draw(TextNotBuy);

			}
			else if (accion == Acciones::SALIRDELACARCEL)
			{
				//Buy text
				window.draw(TextBuy);
				//Price
				window.draw(rectJail);
				window.draw(turnsInJail);
				window.draw(JailPrice);
				//Button not to buy
				window.draw(ButtonNotBuy);
				window.draw(TextNotBuy);
			}
			
		}
		else 
		{
			window.draw(buttonNotMyTurn);
			window.draw(TextThrowDices);
		}
		//PANTALLA DE JUEGO
		window.display();
		window.clear();
	
	}

	return 0;
}