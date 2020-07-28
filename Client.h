#pragma once
#include <algorithm>
#include <iterator>
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h> 
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>



// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


#define DEFAULT_BUFLEN 1024
#define DEFAULT_PORT "6000"

#include <iostream>
#include <string>

class Client {

public:
	std::string username;

	Client(std::string username_in);

	~Client();

	bool clientMain();

private:
	bool sendCommand(std::string command);

	bool rec_msg();

	bool clientClose();

	int clientConnect(std::string username);

	bool isIn(std::string arrayIn[], std::string value);

	SOCKET ConnectSocket;
};
