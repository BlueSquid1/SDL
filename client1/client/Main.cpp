#include <iostream>
#include <SDL.h>
#include <SDL_net.h>
#include <string>

int main(int argc, char** argv)
{
	SDL_Init(SDL_INIT_VIDEO);
	SDLNet_Init();

	IPaddress ip;
	SDLNet_ResolveHost(&ip, "192.168.1.106", 1234);

	TCPsocket client = SDLNet_TCP_Open(&ip);
	char text[100];
	SDLNet_TCP_Recv(client, text, 100);
	std::cout << text;

	SDLNet_TCP_Close(client);

	getchar();
	SDLNet_Quit();
	SDL_Quit();
	return 0;
}