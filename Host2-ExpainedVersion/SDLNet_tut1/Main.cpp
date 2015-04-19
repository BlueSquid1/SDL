//host

#include "CNet.h"

int main(int argc, char ** argv)
{
	CNet::Init();

	charbuf message = "hello";

	CIpAddress IPAddress;

	//inizalize server to be on port 3333
	IPAddress.HostOnPort(3333);

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


	
	//Open a server socket
	CHostSocket HostSocket;
	HostSocket.CreateServerSocket(IPAddress);

	if (HostSocket.Ok())
	{
		std::cout << "Host socket is up\n";
	}
	else
	{
		std::cout << "can't open host socket\n";
	}

	//wait for a client to join
	//--------------------------
	CClientSocket player;
	std::cout << "waiting for client\n";
	while (HostSocket.Accept(player) == false)
	{
		SDL_Delay(100);
	}
	//=========================

	std::cout << "client connected\n";

	/*
	if (HostSocket.Ready() == true)
	{
		//there is information recieved
	}
	*/


	CNet::Quit();
	return 0;
}