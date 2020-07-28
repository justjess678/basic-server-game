#include "Server.h"

char COMMANDS[5] = { 'Q', '8', '4', '6', '2' };
int prevPos[2];
int newPos[2];

Server::Server()
{
	// Place treasure in random spot
	this->treasureLocation[0] = rand() % WIDTH;
	this->treasureLocation[1] = rand() % HEIGHT;
	printf("Treasure is at %d, %d\n", this->treasureLocation[0], this->treasureLocation[1]);
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 5; j++) {
			this->userMap[i][j] = "None";
		}
	}
	printf("Set up map\n");
	if (this->serverConnect()) {
		printf("Connection opened!\n");
	}
	else {
		printf("Error opening connection!\n");
	}
}

Server::~Server()
{
}

bool Server::serverConnect()
{
	WSADATA wsaData;
	int iResult;

	this->ListenSocket = INVALID_SOCKET;
	this->ClientSocket = INVALID_SOCKET;

	struct addrinfo *result = NULL;
	struct addrinfo hints;

	// Initialize Winsock
	printf("Initializing Winsock...\n");
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return 1;
	}

	printf("Initializing hints...\n");
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	printf("Resolving server address and port...\n");
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return false;
	}

	// Create a SOCKET for connecting to server
	printf("Creating a socket...\n");
	this->ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (this->ListenSocket == INVALID_SOCKET) {
		printf("socket failed with error: %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return false;
	}

	// Setup the TCP listening socket
	printf("Setting up listening socket...\n");
	iResult = bind(this->ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(this->ListenSocket);
		WSACleanup();
		return false;
	}

	freeaddrinfo(result);

	printf("Listening to listen socket...\n");
	iResult = listen(this->ListenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		printf("listen failed with error: %d\n", WSAGetLastError());
		closesocket(this->ListenSocket);
		WSACleanup();
		return false;
	}

	// Accept a client socket
	printf("Accepting a client...\n");
	this->ClientSocket = accept(this->ListenSocket, NULL, NULL);
	if (this->ClientSocket == INVALID_SOCKET) {
		printf("accept failed with error: %d\n", WSAGetLastError());
		closesocket(this->ListenSocket);
		WSACleanup();
		return false;
	}
	printf("Done setting up server!\n");
	
	return true;
}

std::string Server::showMap()
{
	std::string out = "";
	for (int i = 0; i < WIDTH; i++) {
		std::string line = "\n-----------------------------------------\n";
		for (int j = 0; j < HEIGHT; j++) {
			line.append(this->userMap[i][j]);
			line.append("\t|");
		}
		out.append(line);
	}
	out.append("\n|||||||||||||||||||||||||||||||||||||||||\n");
	return out;
}

void Server::showPositions() {
	std::map<std::string, int>::iterator it = this->positions_x.begin();
	while (it != this->positions_x.end())
	{
		std::cout << it->first << " :: (" << it->second << ", " << this->positions_y[it->first] << ")" << std::endl;
		it++;
	}
	std::cout << "---------------\n";
}
std::string Server::showScores() {
	std::map<std::string, int>::iterator it = this->scores.begin();
	std::string user = "";
	int score = 0;
	std::string out = "---Scores---\n";
	while (it != this->scores.end())
	{
		user = it->first;
		score = it->second;
		out += user + " : " + std::to_string(score) +  "\n";
		it++;
	}
	out = out.append( "---------------\n");
	return out;
}

bool Server::updateMap(std::string user, int oldPos[], int newPos[]) {
	if (this->userMap[abs(newPos[0] % WIDTH)][abs(newPos[1] % HEIGHT)] == "None") {
		this->userMap[abs(oldPos[0] % WIDTH)][abs(oldPos[1] % HEIGHT)] = "None";
		this->userMap[abs(newPos[0] % WIDTH)][abs(newPos[1] % HEIGHT)] = user;
		return true;
	}
	return false;
}

bool Server::update()
{

	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;

	int iResult = recv(this->ClientSocket, recvbuf, recvbuflen, 0);
	if (iResult > 0) {
		std::string recvbuf2(recvbuf, iResult);
		//printf("Message received: %s\n", recvbuf2.c_str());
		
		if (recvbuf2.find('_') != std::string::npos) {
			std::string username = recvbuf2.substr(recvbuf2.find("_") + 1);
			prevPos[0] = this->positions_x[username];
			prevPos[1] = this->positions_y[username];
			newPos[0] = prevPos[0];
			newPos[1] = prevPos[1];
			char command = recvbuf2.at(0);
			//printf("Command is :%c\n", command );
			
			// "Q" for quit
			if (command == COMMANDS[0]) {
				printf("%s quit the game!\n", username);
				closesocket(this->ClientSocket);
				WSACleanup();
				return false;
			}
			// "4" for left
			else if (command == COMMANDS[2]) {
				this->positions_y.erase(username);
				this->positions_y.insert(std::make_pair(username, (prevPos[1] - 1) % HEIGHT));
			}
			// "2" for down
			else if (command == COMMANDS[4]) {
				this->positions_x.erase(username);
				this->positions_x.insert(std::make_pair(username, (prevPos[0] + 1) % WIDTH));
			}
			// "6" for right
			else if (command == COMMANDS[3]) {
				this->positions_y.erase(username);
				this->positions_y.insert(std::make_pair(username, (prevPos[1] + 1) % HEIGHT));
			}
			// "8" for up
			else if (command == COMMANDS[1]) {
				this->positions_x.erase(username);
				this->positions_x.insert(std::make_pair(username, (prevPos[0] - 1) % WIDTH));
			}
			int old_score = this->scores[username];
			this->scores[username] = old_score - 1;

			//update map and positions
			newPos[0] = this->positions_x[username];
			newPos[1] = this->positions_y[username];

			if (newPos != prevPos) {
				this->updateMap(username, prevPos, newPos);
			}

			if (this->treasureLocation[0] == newPos[0] && this->treasureLocation[1] == newPos[1]) {
				// User won!
				printf("*** %s won the game! ***\n", username.c_str());
				old_score = this->scores[username];
				this->scores[username] = old_score + 10;
			}
		} else {
			// username
			std::string username = recvbuf2;
			printf("%s has joined the game!\n", username.c_str());
			//place randomly or at 0,0 on map
			int initPos[2] = { 0, 0 };
			this->positions_x.insert(std::make_pair(username, initPos[0]));
			this->positions_y.insert(std::make_pair(username, initPos[1]));
			this->scores.insert(std::make_pair(username, 0));
			this->updateMap(username, initPos, initPos);
		}
		printf(this->showScores().c_str());

		// Send the buffer back to the sender
		//std::string result = this->showMap() + this->showScores();
		std::string result = "Potatoes";
		int iSendResult = send(this->ClientSocket, result.c_str(), (int)strlen(result.c_str()), 0);
		if (iSendResult == SOCKET_ERROR) {
			printf("send failed with error: %d\n", WSAGetLastError());
			closesocket(this->ClientSocket);
			WSACleanup();
			return false;
		}
		printf("Bytes sent: %d\n", iSendResult);
	}
	else if (iResult == 0)
		printf("Connection closing...\n");
	else {
		printf("recv failed with error: %d\n", WSAGetLastError());
		closesocket(this->ClientSocket);
		WSACleanup();
		return false;
	}
	

	return true;
}

bool Server::serverClose()
{
	int iResult = shutdown(this->ClientSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(this->ClientSocket);
		WSACleanup();
		return false;
	}

	// cleanup
	closesocket(this->ClientSocket);
	WSACleanup();

	return true;
}

int main()
{
	Server gameServer = Server();
	printf("Server running...");
	do {
		printf("%s", gameServer.showMap().c_str());
	} while (gameServer.update());
}
