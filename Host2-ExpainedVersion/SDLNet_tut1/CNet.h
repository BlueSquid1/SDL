#ifndef CNET_H
#define CNET_H
#include <iostream>
#include "SDL_net.h"

//new variable type. its a 256 array of charactors
using charbuf = char[256];

//starting a closing SDL_Net
class CNet
{

public:
	static bool Init(); //Initialize SDL_Net
	static void Quit(); //exit SDL_Net
};

//handling the data
class CNetMessage
{
protected: //not avaliable in inheridant classes
	charbuf buffer; //limit of 256 charactors
	enum bufstates
	{
		EMPTY, // = 0
		READING, // = 1
		WRITING, // = 2
		FULL // = 3
	};

	//message state
	bufstates state;

public:

	//clear buffer[] and set state to EMPTY
	void Reset();

	//constructor
	CNetMessage();

	//stops it being written to when its is not empty
	virtual int NumToLoad(); //expect to be defined in a inherident class

	//returns length of the string
	virtual int NumToUnload();

	//copy inputbuffer to buffer
	void LoadBytes(charbuf& inputbuffer, int n); //load a char set into the message buffer

	//pop the stored charbuffer to destbuffer
	void UnLoadBytes(charbuf& destbuffer); //unload a char set from the buffer
	void finish(); //set object state to full. no more data to be loaded
};


//handling the connection
class CIpAddress
{
private:
	//contains host and port
	IPaddress m_Ip;
public:
	//default constructor
	CIpAddress();

	//host is the ip address
	void ConnectTo(std::string host, Uint16 port);

	void HostOnPort(Uint16 port);

	//resets host and port
	void Clear();

	//set this object from another CIpAddress
	void SetIp(IPaddress sdl_ip);

	//check if port and host are setup
	bool Ok() const; //will not change any member variables or call any other method

	//return SDL_Net IPaddress structure
	IPaddress GetIpAddress() const;

	//return host
	Uint32 GetHost() const;
	//return port
	Uint16 GetPort() const;
};

//sets up a pipe betwen two computers
class CTcpSocket
{
protected:
	//foreign socket
	TCPsocket m_Socket;

	//stores all the sockets
	SDLNet_SocketSet set;
public:
	//constructor
	CTcpSocket();
	//destructor
	virtual ~CTcpSocket();

	void CloseSocket();

	//set from exisitng socket
	virtual void SetSocket(TCPsocket the_sdl_socket);

	//check if a pipeline to another computer is setup
	bool Ok() const;

	//checks activity in the socket. so know when to read
	bool Ready() const;
	virtual void OnReady();
};


//Client socket setup
class CClientSocket : public CTcpSocket
{
	//Ip of the host
	CIpAddress m_RemoteIp;
public:
	bool Connect(CIpAddress& remoteip);
	//bool Connect(CHostSocket& the_listener_socket);
	void SetSocket(TCPsocket the_sdl_socket);
	CIpAddress getIpAddress() const;
	virtual void OnReady();
	bool Receive(CNetMessage& rData);
	bool Send(CNetMessage& sData);
};

//Host socket setup
class CHostSocket : public CTcpSocket
{
public:
	void CreateServerSocket(CIpAddress& the_ip_address);
	bool Accept(CClientSocket&); //check---------------------
	virtual void OnReady();
};

#endif //CNET_H