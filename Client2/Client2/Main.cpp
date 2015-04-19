//host

#include "CNet.h"

int main(int argc, char ** argv)
{
	CNet::Init();

	charbuf message = "hello";

	CIpAddress IPAddress;

	//inizalize server to be on port 3333
	IPAddress.ConnectTo("192.168.1.106", 3333);

	if (!IPAddress.Ok())
	{
		std::cout << "wasn't able to connect to the host\n";
		//stop the program
		CNet::Quit();
	}
	else
	{
		std::cout << "successfully setup IP\n";
	}





	TCPsocket ClientSocket = SDLNet_TCP_Open(&IPAddress.GetIpAddress());

	/*
	CClientSocket player;

	if (player.Ok())
	{
		std::cout << "Connected to host\n";
	}
	else
	{
		std::cout << "could not find host\n";
	}
	*/



	CNet::Quit();
	return 0;
}