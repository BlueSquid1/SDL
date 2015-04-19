#include <iostream>
#include <string>
#include "SDL.h"
#include "SDL_net.h"


void Close(bool status);


int main(int argc, char ** argv)
{
	IPaddress ip;
	TCPsocket hostSocket;
	char message[255];
	int len;
	std::cout << "Enter IP address of the Host (e.g. 192.168.1.106):";
	std::string IPAddress; // = "192.168.1.106"
	std::cin >> IPAddress;
	Uint16 port = 3333;

	std::cout << "started\n";

	//initialize SDL
	if (SDL_Init(0) == -1)
	{
		std::cout << "Could not start SDL. SDL error: " << SDL_GetError() << std::endl;
		Close(false);
	}

	//initialize SDLNet
	if (SDLNet_Init() == -1)
	{
		std::cout << "Could not start SDL_Net. SDL_Net error: " << SDLNet_GetError() << std::endl;
		Close(false);
	}

	//setup this computer so that it will connect to the server
	if (SDLNet_ResolveHost(&ip, IPAddress.c_str(), port) == -1)
	{
		std::cout << "Could not setup this computer to search for a server. Does it even have networking capabilities? SDL_Net error: " << SDLNet_GetError() << std::endl;
		Close(false);
	}

	//open the host socket (connect to host)
	hostSocket = SDLNet_TCP_Open(&ip);
	while (!hostSocket)
	{
		std::cout << "Can't find server" << std::endl;
		SDL_Delay(2500);
		hostSocket = SDLNet_TCP_Open(&ip);
	}
	std::cout << "found server\n";
	
	bool quit = false;
	while (!quit)
	{
		std::cout << "send message:";
		std::cin >> message;
		len = strlen(message);
		//if len is creater than 0
		if (len)
		{
			if (message == "quit")
			{
				quit = true;
			}
			//add end of line charactor so it is valid
			message[len] = '\0';
			int result = SDLNet_TCP_Send(hostSocket, message, len + 1);
			if (result < len)
			{
				std::cout << "failed to send message\n";
			}
		}
	}

	SDLNet_TCP_Close(hostSocket);

	Close(true);
	return 0;
}



void Close(bool status)
{	
	//close pointers



	if (status == false)
	{
		std::cout << "Critical fail\n";
	}
	else
	{
		std::cout << "done\n";
	}

	SDLNet_Quit();
	SDL_Quit();

	getchar();
}