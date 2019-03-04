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

//Variables Globales
std::mutex mtx;
bool running = true;
sf::Packet packet;
sf::TcpListener listener;
sf::Socket::Status status;

//Lista para guardar los sockets
std::list<sf::TcpSocket*> clients;
//Creamos el selector
sf::SocketSelector selector;

//TCPSocketSelector
void ControlServer()
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
						std::string aux;
						status = client.receive(packReceive);
						if (status == sf::Socket::Done)
						{
							packReceive >> aux;
							std::cout << "He recibido " << aux << " del puerto " << client.getRemotePort() << std::endl;
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
	std::thread t(&ControlServer);
	t.detach();

	//VARIABLES JUEGO
	PlayerInfo playerInfo;

	//BUCLE DE JUEGO
	while (running)
	{

	}
	return 0;
}