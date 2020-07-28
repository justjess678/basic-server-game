#pragma once

#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <map>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>

#include "EACServer.h"
#include "GameServer.h"

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "6000"

#define HEIGHT 5
#define WIDTH 5

#include <iostream>

class Server {
public:
	Server();

	~Server();

	std::string showMap();

	bool update();

private:
	bool serverConnect();

	bool updateMap(std::string user, int prevPos[], int newPos[]);

	void showPositions();

	std::string showScores();

	bool serverClose();

	SOCKET ConnectSocket;
	mutable int treasureLocation[2];
	mutable std::string userMap[5][5];
	std::map<std::string, int> positions_x;
	std::map<std::string, int> positions_y;
	std::map<std::string, int> scores;
	SOCKET ListenSocket;
	SOCKET ClientSocket;

};