#include <iostream>
#include <string>
#include <SDL.h>
#include "SDL_net.h"

//defines CNet, CNetMessage and CIpAddress classes
#include "CNet.h"

bool CNet::Init()
{
	if (SDLNet_Init() < 0)
	{
		std::cout << "SDLNet failed to load. SDLNet error: " << SDLNet_GetError() << std::endl;
		return false;
	}
	return true;
}

void CNet::Quit()
{
	SDLNet_Quit();
	getchar();
}




CNetMessage::CNetMessage()
{
	Reset();
}

void CNetMessage::Reset()
{
	for (int i = 0; i < 256; i++)
	{
		buffer[i] = 0;
	}
	state = EMPTY;
}

void CNetMessage::finish()
{
	//only can be finished after reading
	if (state == READING)
	{
		state = FULL;
	}
}

int CNetMessage::NumToLoad()
{
	if (state == EMPTY)
	{
		return 255;
	}
	else
	{
		return 0;
	}
}

int CNetMessage::NumToUnload()
{
	if (state == FULL)
	{
		//want to delete the string plus the end line charactor
		return strlen(buffer) + 1;
	}
	else
	{
		return 0;
	}
}

void CNetMessage::LoadBytes(charbuf& inputbuffer, int n)
{
	for (int i = 0; i < n; i++)
	{
		buffer[i] = inputbuffer[i];
	}
	state = READING;
}

void CNetMessage::UnLoadBytes(charbuf& destbuffer)
{
	int length = this->NumToUnload();
	for (int i = 0; i < length; i++)
	{
		destbuffer[i] = buffer[i];
	}
	Reset();
}


void CIpAddress::Clear()
{
	m_Ip.host = 0;
	m_Ip.port = 0;
}

CIpAddress::CIpAddress()
{
	Clear();
}

void CIpAddress::ConnectTo(std::string host, Uint16 port)
{
	if (SDLNet_ResolveHost(&m_Ip, host.c_str(), port) < 0)
	{
		std::cerr << "Can't connect to: " << port << ". SDLNet error: " << SDLNet_GetError() << std::endl;
		Clear();
	}
}

void CIpAddress::HostOnPort(Uint16 port)
{
	if (SDLNet_ResolveHost(&m_Ip, NULL, port) < 0)
	{
		std::cerr << "Can't connect to: " << port << ". SDLNet error: " << SDLNet_GetError() << std::endl;
		Clear();
	}
}

bool CIpAddress::Ok() const
{
	//return true if m_Ip has been setup
	return m_Ip.port != 0;
}

void CIpAddress::SetIp(IPaddress sdl_ip)
{
	m_Ip = sdl_ip;
}

IPaddress CIpAddress::GetIpAddress() const
{
	return m_Ip;
}

Uint32 CIpAddress::GetHost() const
{
	return m_Ip.host;
}

Uint16 CIpAddress::GetPort() const
{
	return m_Ip.port;
}



CTcpSocket::CTcpSocket()
{
	m_Socket = NULL;
	//only one socket will be setup (there can only connect to one client)
	set = SDLNet_AllocSocketSet(1);
}

void CTcpSocket::CloseSocket()
{
	if (m_Socket != NULL)
	{
		SDLNet_TCP_DelSocket(set, m_Socket);
		SDLNet_TCP_Close(m_Socket);
	}
}

CTcpSocket::~CTcpSocket()
{
	this->CloseSocket();
}

//set from existing socket
void CTcpSocket::SetSocket(TCPsocket the_sdl_socket)
{
	//before setting the socket close the previous socket
	this->CloseSocket();

	//move to memory
	m_Socket = the_sdl_socket;

	//then setup the socket

	if (SDLNet_TCP_AddSocket(set, m_Socket) == -1)
	{
		std::cerr << "Can't open the socket. SDLNet error: " << SDLNet_GetError() << std::endl;

		//offload from memory
		m_Socket = NULL;
	}

}

bool CTcpSocket::Ok() const
{
	//makes sure the socket is already setup
	return m_Socket != NULL;
}

bool CTcpSocket::Ready() const
{
	bool rd = false;

	//returns how many sockets have new data
	//because we only have 1 socket. if it returns
	//1 then we know that it needs to be read
	int numready = SDLNet_CheckSockets(set, 0); //the 0 means don't wait

	if (numready == -1)
	{
		std::cerr << "Can't check the sockets for data. SDLNet error: " << SDLNet_GetError() << std::endl;
		return false;
	}

	//make sure there is only one active socket
	if (numready == 0)
	{
		std::cerr << "No message\n";
		return false;
	}

	//check is data can be read
	if (SDLNet_SocketReady(m_Socket))
	{
		return true;
	}
	else
	{
		std::cerr << "The message recieved can't be read. SDLNet error:" << SDLNet_GetError() << std::endl;
		return false;
	}
}

void CTcpSocket::OnReady()
{
	//will be defined in an inheridant class
}


void CHostSocket::CreateServerSocket(CIpAddress& the_ip_address)
{
	//initialize the socket
	CTcpSocket::CTcpSocket();

	//duplicate the IP
	IPaddress iph = the_ip_address.GetIpAddress();

	//create a local server socket
	m_Socket = SDLNet_TCP_Open(&iph);

	if (!m_Socket)
	{
		SDLNet_FreeSocketSet(set);
		std::cerr << "Can't create a socket for the ip: " << iph.host << ". SDLNet error: " << SDLNet_GetError() << std::endl;
	}
}


//everything after this point I was a blur------------------------

bool CHostSocket::Accept(CClientSocket& the_client_socket)
{
	TCPsocket cs;
	//accept the incoming client socket
	cs = SDLNet_TCP_Accept(m_Socket);

	if (cs)
	{
		//set socket for client
		the_client_socket.SetSocket(cs);
		return true;
	}
	else
	{
		//no clients to accept
		return false;
	}
}

void CHostSocket::OnReady()
{

}


CClientSocket::CClientSocket()
{
	CTcpSocket::CTcpSocket();
}

CClientSocket::CClientSocket(std::string host, Uint16 port) {
	CIpAddress remoteip;
	remoteip.ConnectTo(host.c_str(), port);
	if (!remoteip.Ok()) {
		m_Socket = NULL;
	}
	else {
		CTcpSocket::CTcpSocket();
		Connect(remoteip);
	}
}

/*
bool CClientSocket::Connect(CHostSocket& the_listener_socket) {
return false;
}
*/
bool CClientSocket::Connect(CIpAddress& remoteip) {
	TCPsocket cs;
	IPaddress ip = remoteip.GetIpAddress();
	if ((cs = SDLNet_TCP_Open(&ip)))
	{
		SetSocket(cs);
		return true;
	}
	else {
		std::cerr << "SDLNet_TCP_Open: " << SDLNet_GetError() << std::endl;
		return false;
	}
}

void CClientSocket::SetSocket(TCPsocket the_sdl_socket) {
	CTcpSocket::SetSocket(the_sdl_socket);
	IPaddress* ips;
	if ((ips = SDLNet_TCP_GetPeerAddress(m_Socket))) {
		/* Print the address, converting it onto the host format */
		m_RemoteIp.SetIp(*ips);
		Uint32 hbo = m_RemoteIp.GetHost();
		Uint16 pbo = m_RemoteIp.GetPort();
		std::cout << "Client connected: " << SDLNet_Read32(&hbo) << ' '
			<< SDLNet_Read16(&pbo) << std::endl;
	}
	else
		std::cerr << "SDLNet_TCP_GetPeerAddress: " << SDLNet_GetError() << std::endl;
}

CIpAddress CClientSocket::getIpAddress() const {
	return m_RemoteIp;
}

bool CClientSocket::Receive(CNetMessage& rData) {
	//Firstly, check if there is a socket
	if (m_Socket == NULL)
		return false;
	charbuf buf;

	//Check if the instance can receive bytes, if it can, load the number of bytes specified by NumToLoad() virtual function
	while (rData.NumToLoad() > 0) {
		if (SDLNet_TCP_Recv(m_Socket, buf, rData.NumToLoad()) > 0) {
			rData.LoadBytes(buf, rData.NumToLoad());
		}
		else {
			return false;
		}
	}
	rData.finish();
	return true;
}

bool CClientSocket::Send(CNetMessage& sData) {
	//check if there is a socket
	if (m_Socket == NULL)
		return false;
	charbuf buf;
	int len;

	//Check if the instance can send bytes, if it can, unload the number of bytes specified by NumToLoad() virtual function
	while ((len = sData.NumToUnload()) > 0) {
		sData.UnLoadBytes(buf);
		if (SDLNet_TCP_Send(m_Socket, (void *)buf, len) < len) {
			std::cerr << "SDLNet_TCP_Send: " << SDLNet_GetError() << std::endl;
			return false;
		}
	}
	return true;
}

void CClientSocket::OnReady() {

}