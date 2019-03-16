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

//Variables Globales
std::mutex mtx;
bool running = true;
bool StartedGame = false;
sf::Packet packet;
sf::TcpListener listener;
sf::Socket::Status status;
//NumTurn
int indexTurn;

//Lista para guardar los sockets
std::list<sf::TcpSocket*> clients;
//Creamos el selector
sf::SocketSelector selector;

//ENUM (ORDENES DEL CLIENTE)
enum Ordenes
{
	StartGame
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

//TCPSocketSelector
void ControlServer()
{
	status = listener.listen(PORT);
	if (status != sf::Socket::Done)
	{
		std::cout << "Error al abrir listener\n";
		exit(0);
	}
	//A�adimos el listener
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
					std::cout << "Error al recoger conexi�n nueva\n";
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
						int order;
						status = client.receive(packReceive);
						if (status == sf::Socket::Done)
						{
							packReceive >> order;
							std::cout << "He recibido la orden" << order << " del puerto " << client.getRemotePort() << std::endl;
							switch (order)
							{
							case 0:
								
								break;
							case 1:
								break;
							case 2:
								break;
							case 3:
								break;
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

//Peer-To-Peer



int main()
{
	//VARIABLES CONEXION
	sf::TcpSocket sock;
	sf::TcpSocket::Status status;

	//CONEXION CLIENTE-SERVIDOR
	/*std::thread t(&ControlServer);
	t.detach();*/

	//VECTOR DE PLAYES
	std::vector<PlayerInfo> players;

	//VARIABLES JUEGO
	PlayerInfo playerInfo;
	int NumJugadores;
	int counterPlayer = 0;
	
	//Ordenes
	int order;

	//BUCLE DE JUEGO
	while (running)
	{
		status = listener.listen(PORT);
		if (status != sf::Socket::Done)
		{
			std::cout << "Error al abrir listener\n";
			exit(0);
		}
		//A�adimos el listener
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
						std::cout << "Error al recoger conexi�n nueva\n";
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

									//A�ADIMOS JUGADOR AL VECTOR
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
									std::cout << indexTurn << std::endl;
									break;
								case 2:
									break;
								case 3:
									break;
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