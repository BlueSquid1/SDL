#include <iostream>
#include <SDL.h>
#include <SDL_net.h>
#include <string>

int main(int argc, char** argv)
{
	SDL_Init(SDL_INIT_VIDEO);
	SDLNet_Init();

	IPaddress ip;
	SDLNet_ResolveHost(&ip, NULL, 1234); //NULL meanas it is a server

	TCPsocket server = SDLNet_TCP_Open(&ip);
	TCPsocket client;

	while (1)
	{
		client = SDLNet_TCP_Accept(server);
		if (client)
		{
			//commicate with client
			const char* text = "Hello Client";
			SDLNet_TCP_Send(client, text, strlen(text) + 1);
			SDLNet_TCP_Close(client);
			break;
		}
	}

	SDLNet_TCP_Close(server);

	SDLNet_Quit();
	SDL_Quit();
	return 0;
}