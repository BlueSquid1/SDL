#include <iostream>
#include "SDL.h"
#include "SDL_net.h"


void Close(bool status);


int main(int argc, char ** argv)
{
	IPaddress ip, *remoteip;
	TCPsocket server = NULL;
	TCPsocket client = NULL;
	char message[255];
	int len;
	Uint32 ipaddr;
	Uint16 port = 3333;
	//SDLNet_SocketSet set; //check if this is actually needed

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

	//setup the computer as a host
	if (SDLNet_ResolveHost(&ip, NULL, port) == -1)
	{
		std::cout << "Could not setup this computer has a host. Does it even have networking capabilities? SDL_Net error: " << SDLNet_GetError() << std::endl;
		Close(false);
	}

	//open the server socket
	server = SDLNet_TCP_Open(&ip);

	if (!server)
	{
		std::cout << "Could not open the port. stupid firewall. SDL_Net error: " << SDLNet_GetError() << std::endl;
		Close(false);
	}

	//get Host details
	IPaddress hostIP;
	hostIP.host = NULL;
	const char* tempstr = SDLNet_ResolveIP(&hostIP);
	SDLNet_ResolveHost(&hostIP, tempstr, 3333);
	Uint32 hostAd = SDL_SwapBE32(hostIP.host);
	std::cout << "Host IP address is: " << (hostAd >> 24) << "." << ((hostAd >> 16) & 0xff) << "." << ((hostAd >> 8) & 0xff) << "." << (hostAd & 0xff) << std::endl;

	
	bool connected = false;

	bool quit = false;
	while (!quit)
	{
		//if it ever drops try to recover
		if (connected == false)
		{
			//try to accept any incoming connection
			client = SDLNet_TCP_Accept(server);
			if (!client)
			{
				std::cout << "waiting for client\n";
				SDL_Delay(3000);
			}
			else
			{
				//connected to Client
				//get client IP address and port number
				remoteip = SDLNet_TCP_GetPeerAddress(client);
				if (!remoteip)
				{
					std::cout << "can't find details about client. SDLNet error: " << SDLNet_GetError() << std::endl;
				}
				else
				{
					connected = true;
					//print out client's IP address and port number
					ipaddr = SDL_SwapBE32(remoteip->host);
					std::cout << "Connecting to IP: " << (ipaddr >> 24) << "." << ((ipaddr >> 16) & 0xff) << "." << ((ipaddr >> 8) & 0xff) << "." << (ipaddr & 0xff) << " port: " << remoteip->port << std::endl;
				}
			}
		}
		else // connected == true
		{
			//check if there are any new messages
			len = SDLNet_TCP_Recv(client, message, 255);
			//len = -1 means no new messages
			//len = 0 means critical error
			if (len == 0)
			{
				std::cout << "Corrupt Message recieved\n";
			}
			else if (len > 0)
			{
				std::cout << message << std::endl;
				if (message[0] == 'q')
				{
					quit = true;
				}
			}
		}
	}

	SDLNet_TCP_Close(client);

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