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

//MENSAJES DEL SERVIDOR
void RecepcionMensaje(sf::TcpSocket* sock)
{
	sf::Packet packReceive;
	while (running)
	{
		sf::Packet packReceive;
		std::string aux;
		sock->receive(packReceive);
		packReceive >> aux;

		//COMPROBAMOS SI HA LLEGADO ALGO
		std::cout << aux << std::endl;
	}
}

int main()
{
	//VARIABLES
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

		//SEND DE COMPROVACION
		/*pack << "Cliente conectado tt";
		sock.send(pack);*/
	}

	//BUCLE DE JUEGO
	while (running)
	{

	}


	PlayerInfo playerInfo;
	return 0;
}