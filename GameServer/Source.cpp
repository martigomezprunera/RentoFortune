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
//enum
enum tipoCasilla
{
	PROPIEDAD,
	ESTACION,
	NEUTRA,
	FREEMONEY,
	TAX,
	JAIL,
	COMPANY
};
//struct casilla
struct casilla 
{
	tipoCasilla tipo;
	std::string name;
	int numCasas;
	int price;
	int priceEdification;
	int mortgageValue;
	int owner;
	int priceToCharge = 50;
};

//Variables Globales
std::mutex mtx;
bool running = true;
bool StartedGame = false;
sf::Packet packet;
sf::TcpListener listener;
sf::Socket::Status status;
casilla tablero[40];
int perderTurno[4] = { 0,0,0,0 };
//VECTOR DE PLAYES
std::vector<PlayerInfo> players;
//NumTurn
int indexTurn;
sf::Vector2<int> auxPosition;

//Dices
int dice1;
int dice2;
int resultDices;
int indexUpdateMoney = -1;;
float casillasTotales;
int RandomToCharge;
int Tax;
int EntireRound = 0;

//Lista para guardar los sockets
std::list<sf::TcpSocket*> clients;
//Creamos el selector
sf::SocketSelector selector;

//ENUM (ORDENES DEL CLIENTE)
enum Ordenes
{
	StartGame,
	ResultDice,
	UpdateMoney,
	NewTurn,
	Charge,
	RoundMoney,
	WantGetOut
};

//OVERCHARGED FUNCTIONS (PLAYER INFO)
sf::Packet& operator <<(sf::Packet& packet, const PlayerInfo& playerInfo)
{
	return packet << playerInfo.name << playerInfo.position.x << playerInfo.position.y << playerInfo.money;
}
sf::Packet& operator >>(sf::Packet& packet, PlayerInfo& playerInfo)
{
	return packet >> playerInfo.name >> playerInfo.position.x >> playerInfo.position.y >> playerInfo.money;
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
//Function calculate casilla
sf::Vector2<int> PositionCasilla(int numCasilla)
{
	sf::Vector2<int> auxVec;
	int aux;
	if (numCasilla <= 10)
	{
		aux = numCasilla % 11;
		auxVec.y = 540;
		for (int i = 0; i < 4; i++)
		{
			if (players[i].casilla == numCasilla)
			{
				auxVec.y += 5;
			}
		}
		
		auxVec.x = 550 - (aux * 51);
	}
	else if (numCasilla<=20)
	{
		aux = (numCasilla-10) % 11;
		auxVec.x = 40;
		for (int i = 0; i < 4; i++)
		{
			if (players[i].casilla == numCasilla)
			{
				auxVec.x -= 5;
			}
		}
		auxVec.y = 553 - (aux * 51);
	}
	else if (numCasilla <= 30)
	{
		aux = (numCasilla - 20) % 11;
		auxVec.y = 40;
		for (int i = 0; i < 4; i++)
		{
			if (players[i].casilla == numCasilla)
			{
				auxVec.y -= 5;
			}
		}

		auxVec.x = 40 + (aux * 51);
	}
	else
	{
		aux = (numCasilla - 30) % 11;
		auxVec.x = 549;
		for (int i = 0; i < 4; i++)
		{
			if (players[i].casilla == numCasilla)
			{
				auxVec.x += 5;
			}
		}
		auxVec.y = 40 + (aux * 51);
	}
	return auxVec;
}
int getTypeCasilla(int numCasilla)
{
	return tablero[numCasilla].tipo;
}
//Funcion tablero
void initializeBoard()
{
	for (int i = 0; i < 40; i++)
	{
		tablero[i].numCasas = 0;
		switch (i)
		{
		case 0:		
			tablero[i].tipo = tipoCasilla::NEUTRA;
			break;
		case 1:
			tablero[i].price = 60;
			tablero[i].owner = -1;
			tablero[i].name = "Primera Marron";
			tablero[i].priceEdification = 100;
			tablero[i].tipo = tipoCasilla::PROPIEDAD;
			tablero[i].priceToCharge = 2;
			break;
		case 2:
			tablero[i].tipo = tipoCasilla::FREEMONEY;
			break;
		case 3:
			tablero[i].price = 60;
			tablero[i].owner = -1;
			tablero[i].name = "Segunda Marron";
			tablero[i].priceEdification = 100;
			tablero[i].tipo = tipoCasilla::PROPIEDAD;
			tablero[i].priceToCharge = 4;
			break;
		case 4:
			//tablero[i].price = 100;
			tablero[i].tipo = tipoCasilla::TAX;
			tablero[i].priceToCharge = 200;
			break;
		case 5:
			tablero[i].price = 200;
			tablero[i].owner = -1;
			tablero[i].name = "Primera Estacion";
			tablero[i].tipo = tipoCasilla::ESTACION;
			tablero[i].priceToCharge = 50;
			break;
		case 6:
			tablero[i].price = 100;
			tablero[i].owner = -1;
			tablero[i].name = "Primera Azul";
			tablero[i].priceEdification = 140;
			tablero[i].tipo = tipoCasilla::PROPIEDAD;
			tablero[i].priceToCharge = 6;
			break;
		case 7:
			tablero[i].tipo = tipoCasilla::FREEMONEY;
			break;
		case 8:
			tablero[i].price = 100;
			tablero[i].owner = -1;
			tablero[i].name = "Segunda Azul";
			tablero[i].priceEdification = 140;
			tablero[i].tipo = tipoCasilla::PROPIEDAD;
			tablero[i].priceToCharge = 6;
			break;
		case 9:
			tablero[i].price = 120;
			tablero[i].owner = -1;
			tablero[i].name = "Tercera Azul";
			tablero[i].priceEdification = 160;
			tablero[i].tipo = tipoCasilla::PROPIEDAD;
			tablero[i].priceToCharge = 8;
			break;
		case 10:
			tablero[i].tipo = tipoCasilla::NEUTRA;
			break;
		case 11:
			tablero[i].price = 140;
			tablero[i].owner = -1;
			tablero[i].name = "Primera Rosa";
			tablero[i].priceEdification = 180;
			tablero[i].tipo = tipoCasilla::PROPIEDAD;
			tablero[i].priceToCharge = 10;
			break;
		case 12:
			tablero[i].price = 150;
			tablero[i].owner = -1;
			tablero[i].name = "Luz";
			tablero[i].tipo = tipoCasilla::COMPANY;
			tablero[i].priceToCharge = 4;
			break;
		case 13:
			tablero[i].price = 140;
			tablero[i].owner = -1;
			tablero[i].name = "Segunda Rosa";
			tablero[i].priceEdification = 180;
			tablero[i].tipo = tipoCasilla::PROPIEDAD;
			tablero[i].priceToCharge = 10;
			break;
		case 14:
			tablero[i].price = 160;
			tablero[i].owner = -1;
			tablero[i].name = "Tercera Rosa";
			tablero[i].priceEdification = 200;
			tablero[i].tipo = tipoCasilla::PROPIEDAD;
			tablero[i].priceToCharge = 12;
			break;
		case 15:
			tablero[i].price = 200;
			tablero[i].owner = -1;
			tablero[i].name = "Segunda Estacion";
			tablero[i].tipo = tipoCasilla::ESTACION;
			tablero[i].priceToCharge = 50;
			break;
		case 16:
			tablero[i].price = 180;
			tablero[i].owner = -1;
			tablero[i].name = "Primera Naranja";
			tablero[i].priceEdification = 220;
			tablero[i].tipo = tipoCasilla::PROPIEDAD;
			tablero[i].priceToCharge = 14;
			break;
		case 17:
			tablero[i].tipo = tipoCasilla::FREEMONEY;
			break;
		case 18:
			tablero[i].price = 180;
			tablero[i].owner = -1;
			tablero[i].name = "Segunda Naranja";
			tablero[i].priceEdification = 220;
			tablero[i].tipo = tipoCasilla::PROPIEDAD;
			tablero[i].priceToCharge = 14;
			break;
		case 19:
			tablero[i].price = 200;
			tablero[i].owner = -1;
			tablero[i].name = "Tercera Naranja";
			tablero[i].priceEdification = 240;
			tablero[i].tipo = tipoCasilla::PROPIEDAD;
			tablero[i].priceToCharge = 16;
			break;
		case 20:
			tablero[i].tipo = tipoCasilla::NEUTRA;
			break;
		case 21:
			tablero[i].price = 220;
			tablero[i].owner = -1;
			tablero[i].name = "Primera Roja";
			tablero[i].priceEdification = 260;
			tablero[i].tipo = tipoCasilla::PROPIEDAD;
			tablero[i].priceToCharge = 18;
			break;
		case 22:
			tablero[i].tipo = tipoCasilla::FREEMONEY;
			break;
		case 23:
			tablero[i].price = 220;
			tablero[i].owner = -1;
			tablero[i].name = "Segunda Roja";
			tablero[i].priceEdification = 260;
			tablero[i].tipo = tipoCasilla::PROPIEDAD;
			tablero[i].priceToCharge = 18;
			break;
		case 24:
			tablero[i].price = 240;
			tablero[i].owner = -1;
			tablero[i].name = "Tercera Roja";
			tablero[i].priceEdification = 280;
			tablero[i].tipo = tipoCasilla::PROPIEDAD;
			tablero[i].priceToCharge = 20;
			break;
		case 25:
			tablero[i].price = 200;
			tablero[i].owner = -1;
			tablero[i].name = "Tercera Estacion";
			tablero[i].tipo = tipoCasilla::ESTACION;
			tablero[i].priceToCharge = 50;
			break;
		case 26:
			tablero[i].price = 260;
			tablero[i].owner = -1;
			tablero[i].name = "Primera Amarilla";
			tablero[i].priceEdification = 300;
			tablero[i].tipo = tipoCasilla::PROPIEDAD;
			tablero[i].priceToCharge = 22;
			break;
		case 27:
			tablero[i].price = 260;
			tablero[i].owner = -1;
			tablero[i].name = "Segunda Amarilla";
			tablero[i].priceEdification = 300;
			tablero[i].tipo = tipoCasilla::PROPIEDAD;
			tablero[i].priceToCharge = 22;
			break;
		case 28:
			tablero[i].price = 150;
			tablero[i].owner = -1;
			tablero[i].name = "Agua";
			tablero[i].tipo = tipoCasilla::COMPANY;
			tablero[i].priceToCharge = 4;
			break;
		case 29:
			tablero[i].price = 280;
			tablero[i].owner = -1;
			tablero[i].name = "Tercera Amarilla";
			tablero[i].priceEdification = 320;
			tablero[i].tipo = tipoCasilla::PROPIEDAD;
			tablero[i].priceToCharge = 24;
			break;
		case 30:
			tablero[i].tipo = tipoCasilla::JAIL;
			break;
		case 31:
			tablero[i].price = 300;
			tablero[i].owner = -1;
			tablero[i].name = "Primera Verde";
			tablero[i].priceEdification = 340;
			tablero[i].tipo = tipoCasilla::PROPIEDAD;
			tablero[i].priceToCharge = 26;
			break;
		case 32:
			tablero[i].price = 300;
			tablero[i].owner = -1;
			tablero[i].name = "Segunda Verde";
			tablero[i].priceEdification = 340;
			tablero[i].tipo = tipoCasilla::PROPIEDAD;
			tablero[i].priceToCharge = 26;
			break;
		case 33:
			tablero[i].tipo = tipoCasilla::FREEMONEY;
			break;
		case 34:
			tablero[i].price = 320;
			tablero[i].owner = -1;
			tablero[i].name = "Tercera Verde";
			tablero[i].priceEdification = 340;
			tablero[i].tipo = tipoCasilla::PROPIEDAD;
			tablero[i].priceToCharge = 28;
			break;
		case 35:
			tablero[i].price = 200;
			tablero[i].owner = -1;
			tablero[i].name = "Cuarta Estacion";
			tablero[i].tipo = tipoCasilla::ESTACION;
			tablero[i].priceToCharge = 50;
			break;
		case 36:
			tablero[i].tipo = tipoCasilla::FREEMONEY;
			break;
		case 37:
			tablero[i].price = 350;
			tablero[i].owner = -1;
			tablero[i].name = "Primera Azul";
			tablero[i].priceEdification = 390;
			tablero[i].tipo = tipoCasilla::PROPIEDAD;
			tablero[i].priceToCharge = 35;
			break;
		case 38:
			tablero[i].price = 100;
			tablero[i].tipo = tipoCasilla::TAX;
			tablero[i].priceToCharge = 100;
			break;
		case 39:
			tablero[i].price = 400;
			tablero[i].owner = -1;
			tablero[i].name = "Segunda Azul";
			tablero[i].priceEdification = 440;
			tablero[i].tipo = tipoCasilla::PROPIEDAD;
			tablero[i].priceToCharge = 50;
			break;		
		default:
			break;
		}
	}
}
//Funcion para saber cuanto ha de pagar alguien que ha caido en compañias
int calculateCompanyPriceToCharge(int sumDices)
{
	if(tablero[12].owner== tablero[28].owner)
		return(sumDices * 10);
	else
		return (sumDices * 4);
}

int main()
{
	//RANDOM
	srand(time(NULL));

	//VARIABLES CONEXION
	sf::TcpSocket sock;
	sf::TcpSocket::Status status;

	//CONEXION CLIENTE-SERVIDOR
	/*std::thread t(&ControlServer);
	t.detach();*/

	//Board
	initializeBoard();

	//VARIABLES JUEGO
	PlayerInfo playerInfo;
	int NumJugadores;
	int counterPlayer = 0;
	bool finishTurn = false;
	//Ordenes
	int order;
	bool haveToSend = false;
	bool auxPlayerBuy=false;
	int auxMoneyToCharge;
	//BUCLE DE JUEGO
	while (running)
	{
		status = listener.listen(PORT);
		if (status != sf::Socket::Done)
		{
			std::cout << "Error al abrir listener\n";
			exit(0);
		}
		//Añadimos el listener
		selector.add(listener);

		while (running)
		{
			// Make the selector wait for data on any socket
			if (selector.wait())
			{
				// Test the listener
				if (selector.isReady(listener))
				{
					// The listener is ready: there is a pending connection
					sf::TcpSocket* client = new sf::TcpSocket;
					if (listener.accept(*client) == sf::Socket::Done)
					{
						// Add the new client to the clients list
						std::cout << "Llega el cliente con puerto: " << client->getRemotePort() << std::endl;
						clients.push_back(client);
						// Add the new client to the selector so that we will
						// be notified when he sends something
						selector.add(*client);
					}
					else
					{
						// Error, we won't get a new connection, delete the socket
						std::cout << "Error al recoger conexión nueva\n";
						delete client;
					}
				}
				else
				{
					// The listener socket is not ready, test all other sockets (the clients)
					for (std::list<sf::TcpSocket*>::iterator it = clients.begin(); it != clients.end(); ++it)
					{
						sf::TcpSocket& client = **it;
						if (selector.isReady(client))
						{
							// The client has sent some data, we can receive it
							sf::Packet packReceive;
							sf::Packet packSend;
							status = client.receive(packReceive);
							if (status == sf::Socket::Done)
							{

								packReceive >> order;
								std::cout << "He recibido la orden " << order << " del puerto " << client.getRemotePort() << std::endl;

								switch (order)
								{
								case 0://CreatePlayer///////
									counterPlayer++;
									packReceive >> playerInfo.name;
									playerInfo.id = counterPlayer;

									//AÑADIMOS JUGADOR AL VECTOR
									mtx.lock();
									players.push_back(playerInfo);
									mtx.unlock();

									//COMPROBAMOS JUGADOR
									/*for (int i = 0; i < players.size(); i++)
									{
										std::cout << players[i].id << std::endl;
										std::cout << players[i].name << std::endl;
										std::cout << players[i].position.x << std::endl;
										std::cout << players[i].position.y << std::endl;
										std::cout << players[i].money << std::endl;
										std::cout << players[i].isYourTurn << std::endl;
									}*/

									break;
								case 1://ThrowDice///////
									packReceive >> indexTurn;	
									//Look if he can move
									if (perderTurno[indexTurn] > 0)
									{
										packSend.clear();
										packSend << Ordenes::WantGetOut;
										perderTurno[indexTurn]--;
										packSend << perderTurno[indexTurn];
									}
									else {
										//RANDOM DICES
										dice1 = rand() % 6 + 1;
										dice2 = rand() % 6 + 1;
										resultDices = 10;// dice1 + dice2;
										//casillasTotales = resultDices / 10;
										players[indexTurn].casilla += resultDices;
										if (players[indexTurn].casilla > 39)
										{
											if (players[indexTurn].casilla == 40)
											{
												players[indexTurn].casilla = 0;
											}
											else
											{
												players[indexTurn].casilla = (players[indexTurn].casilla % 40);
												players[indexTurn].money += 200;
												EntireRound = 200;
												indexUpdateMoney = indexTurn;
											}
										}
										//CALCULAMOS POSICION SEGUN LA CASILLA
										auxPosition = PositionCasilla(players[indexTurn].casilla);
										//rellenamos pack
										packSend.clear();
										packSend << Ordenes::ResultDice;
										packSend << indexTurn;
										packSend << players[indexTurn].casilla;
										packSend << auxPosition.x;
										packSend << auxPosition.y;
										packSend << tablero[players[indexTurn].casilla].tipo;
										switch (getTypeCasilla(players[indexTurn].casilla))
										{
										case 0://Propiedad
											packSend << tablero[players[indexTurn].casilla].owner;
											if ((tablero[players[indexTurn].casilla].owner == -1) && (players[indexTurn].money > tablero[players[indexTurn].casilla].price))//Casilla sin dueño
											{
												//Se envia una pregunta para comprar
												packSend << tablero[players[indexTurn].casilla].price;
												std::cout << "price: " << tablero[players[indexTurn].casilla].price;
												//Enviar precio al que cobras
											}
											else
											{
												//Se le envia la cantidad de dinero que le queda y al jugador beneficiado la suya						
												auxMoneyToCharge = players[indexTurn].money - tablero[players[indexTurn].casilla].priceToCharge;
												packSend << auxMoneyToCharge;
												auxMoneyToCharge = players[tablero[players[indexTurn].casilla].owner].money + tablero[players[indexTurn].casilla].priceToCharge;
												packSend << auxMoneyToCharge;
												packSend << tablero[players[indexTurn].casilla].owner;
												finishTurn = true;
											}
											break;
										case 1://Estacion
											packSend << tablero[players[indexTurn].casilla].owner;
											if ((tablero[players[indexTurn].casilla].owner == -1) && (players[indexTurn].money > tablero[players[indexTurn].casilla].price))//Casilla sin dueño
											{
												//Se envia una pregunta para comprar
												packSend << tablero[players[indexTurn].casilla].price;
												std::cout << "price: " << tablero[players[indexTurn].casilla].price;
												//Enviar precio al que cobras
											}
											else
											{
												//Se le envia la cantidad de dinero que le queda y al jugador beneficiado la suya						
												auxMoneyToCharge = players[indexTurn].money - tablero[players[indexTurn].casilla].priceToCharge;
												packSend << auxMoneyToCharge;
												auxMoneyToCharge = players[tablero[players[indexTurn].casilla].owner].money + tablero[players[indexTurn].casilla].priceToCharge;
												packSend << auxMoneyToCharge;
												packSend << tablero[players[indexTurn].casilla].owner;
												finishTurn = true;
											}
											break;
										case 2://Neutra
											finishTurn = true;
											break;
										case 3://FreeMoney
											//RandomMoneyToGive y enviar que cantidad tiene ahora
											std::cout << "FREE MONEY" << std::endl;
											players[indexTurn].money = (rand() % 200 + 1) + players[indexTurn].money;
											packSend << players[indexTurn].money;
											finishTurn = true;
											//EntireRound = 0;
											break;
										case 4://Tax
											//Envio de cantidad que le queda al jugador
											std::cout << "HACIENDA" << std::endl;
											players[indexTurn].money = players[indexTurn].money - tablero[players[indexTurn].casilla].priceToCharge;
											packSend << players[indexTurn].money;
											finishTurn = true;
											//EntireRound = 0;
											break;
										case 5://Jail
											players[indexTurn].casilla = 10;
											auxPosition = PositionCasilla(players[indexTurn].casilla);
											packSend << players[indexTurn].casilla;
											packSend << auxPosition.x;
											packSend << auxPosition.y;
											finishTurn = true;
											//Se le informa de que estara en la carcel x turnos si no paga
											perderTurno[indexTurn] = 3;
											break;
										case 6://Company
											packSend << tablero[players[indexTurn].casilla].owner;
											if (tablero[players[indexTurn].casilla].owner == -1)//Casilla sin dueño
											{
												//Se envia una pregunta para comprar
													//Se envia una pregunta para comprar
												packSend << tablero[players[indexTurn].casilla].price;
												std::cout << "price: " << tablero[players[indexTurn].casilla].price;
												//Enviar precio al que cobras
											}
											else
											{
												//Se le envia la cantidad de dinero que le queda y al jugador beneficiado la suya
													//Se le envia la cantidad de dinero que le queda y al jugador beneficiado la suya						
												auxMoneyToCharge = players[indexTurn].money - calculateCompanyPriceToCharge(resultDices);
												std::cout << "Dados:" << resultDices << "Precio a pagar:" << calculateCompanyPriceToCharge(resultDices) << std::endl;
												packSend << auxMoneyToCharge;
												auxMoneyToCharge = players[tablero[players[indexTurn].casilla].owner].money + calculateCompanyPriceToCharge(resultDices);
												packSend << auxMoneyToCharge;
												packSend << tablero[players[indexTurn].casilla].owner;
												finishTurn = true;
											}
											break;
										default:
											break;
										}
										//Ponemos a true bool para enviar
										haveToSend = true;
										break;
									}
								case 2://DecideBUY
									packReceive >> indexTurn;
									packReceive >> auxPlayerBuy;
									std::cout << "playerBuy: " << auxPlayerBuy << std::endl;									
									if (auxPlayerBuy)
									{										
										tablero[players[indexTurn].casilla].owner = indexTurn;
										players[indexTurn].money = players[indexTurn].money - tablero[players[indexTurn].casilla].price;
										//RellenamosSend
										packSend.clear();
										packSend << Ordenes::UpdateMoney;										
										packSend << players[indexTurn].money;										
										std::cout << "Player decide buy House" << std::endl;
										//EntireRound = 0;
									}	
									else 
									{
										//FALTA ENVIAR LA PASTA SI HA HECHO UNA VUELTA ENTERA
										packSend << Ordenes::NewTurn;
									}
									haveToSend = true;
									finishTurn = true;
									break;
								case 3://DecideJail
									packReceive >> indexTurn;
									packReceive >> auxPlayerBuy;
									std::cout << "Player turn: " << indexTurn << std::endl;
									if (auxPlayerBuy)
									{
										perderTurno[indexTurn] = 0;
										players[indexTurn].money = players[indexTurn].money - 50;
										//RellenamosSend
										packSend.clear();
										packSend << Ordenes::RoundMoney;
										packSend << players[indexTurn].money;
										packSend << indexTurn;
										std::cout << "Player decide buy House" << std::endl;
										//EntireRound = 0;
									}
									else
									{
										//FALTA ENVIAR LA PASTA SI HA HECHO UNA VUELTA ENTERA
										perderTurno[indexTurn]--;
										packSend << Ordenes::NewTurn;										
										packSend << perderTurno[indexTurn];
										std::cout << "Turnos restantes :" << perderTurno[indexTurn] << std::endl;
										finishTurn = true;
									}
									haveToSend = true; 
									break;
								}

								if (finishTurn == true)
								{
									packSend << indexTurn;
									players[indexTurn].isYourTurn = false;
									indexTurn++;									
									if (indexTurn > 3)
									{
										indexTurn = 0;										
									}									
									players[indexTurn].isYourTurn = true;									
									packSend << indexTurn;
									std::cout << "Next Turn id :" << indexTurn;
									finishTurn = false;									
								}

								if (haveToSend)
								{

									for (std::list<sf::TcpSocket*>::iterator it = clients.begin(); it != clients.end(); ++it)
									{
										sf::TcpSocket& client = **it;
										status = client.send(packSend);
										if (status == sf::Socket::Done)
										{
											std::cout << "Paquete Enviado " << std::endl;
										}
									}									
									haveToSend = false;
								}

								if (EntireRound!=0)
								{
									packSend.clear();
									packSend << Ordenes::RoundMoney;
									packSend << players[indexUpdateMoney].money;
									packSend << indexUpdateMoney;
									for (std::list<sf::TcpSocket*>::iterator it = clients.begin(); it != clients.end(); ++it)
									{
										sf::TcpSocket& client = **it;
										status = client.send(packSend);
										if (status == sf::Socket::Done)
										{
											std::cout << "Paquete Enviado " << std::endl;
										}
									}
									EntireRound = 0;
								}

								//NUMERO DE JUGADORES EN PARTIDA
								NumJugadores = players.size();
								if (NumJugadores == 4)
								{

									for (std::list<sf::TcpSocket*>::iterator it = clients.begin(); it != clients.end(); ++it)
									{
										sf::TcpSocket& client = **it;
										
										//START GAME
										if (StartedGame == false)
										{

											packSend << NumJugadores;
											//ENVIO DE TODA LA INFORMACION DE LOS JUGADORES
											for (int i = 0; i < NumJugadores; i++)
											{
												packSend << players[i].id;
												packSend << players[i].name;

												switch (i)
												{
												case 0:
													players[i].position.x = 550;
													players[i].position.y = 545;
													break;
												case 1:
													players[i].position.x = 525;
													players[i].position.y = 570;
													break;
												case 2:
													players[i].position.x = 550;
													players[i].position.y = 570;
													break;
												case 3:
													players[i].position.x = 575;
													players[i].position.y = 570;
													break;
												}

												packSend << players[i].position.x;
												packSend << players[i].position.y;
												packSend << players[i].money;
												//SI ES EL JUGADOR ID 1 EMPIEZA
												if (i == 0) 
												{
													players[i].isYourTurn = true;
												}
												//SI NO ES EL JUGADOR ID 1 NO EMPIEZA
												else
												{
													players[i].isYourTurn = false;
												}

												packSend << players[i].isYourTurn;
												packSend << players[i].casilla;
												

												std::cout << players[i].name << std::endl;
												std::cout << players[i].position.x << std::endl;
												std::cout << players[i].position.y << std::endl;
												std::cout << players[i].money << std::endl;
												std::cout << players[i].isYourTurn << std::endl;

											}

											//ENVIAMOS PACKET
											status = client.send(packSend);
											if (status == sf::Socket::Done)
											{
												std::cout << "He enviado la informacion de los jugadores" << std::endl;
												std::cout << "He enviado que empiece el juego" << std::endl;
											}
										}
									}
									StartedGame = true;
								}
								
							}
							else if (status == sf::Socket::Disconnected)
							{
								selector.remove(client);
								std::cout << "Elimino el socket que se ha desconectado\n";
							}
							else
							{
								std::cout << "Error al recibir de " << client.getRemotePort() << std::endl;
							}
						}
					}
				}
			}
		}
	}
	return 0;
}