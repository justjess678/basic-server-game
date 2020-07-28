#include "Client.h"


std::string COMMANDS[5] = { "Q", "8", "4", "6", "2" };

Client::Client(std::string username_in) {
	username = username_in;

	if (this->clientConnect(username)) {
		std::cout << "Connection to server successful!\n";
		// Send an initial buffer
		const char *sendbuf = username.c_str();
		int iResult = send(this->ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
		if (iResult == SOCKET_ERROR) {
			printf("send failed with error: %d\n", WSAGetLastError());
			closesocket(ConnectSocket);
			WSACleanup();
		}
	}
	else {
		std::cout << "Error connecting to server!\n";
	}
}

Client::~Client()
{
}

int Client::clientConnect(std::string username) {
	WSADATA wsaData;
	SOCKET ConnectSocket = INVALID_SOCKET;
	struct addrinfo *result = NULL,
		*ptr = NULL,
		hints;
	const char *sendbuf = "this is a test";
	int iResult;


	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port
	iResult = getaddrinfo("127.0.0.1", DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	// Attempt to connect to an address until one succeeds
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

		// Create a SOCKET for connecting to server
		this->ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
			ptr->ai_protocol);
		if (this->ConnectSocket == INVALID_SOCKET) {
			printf("socket failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			return 1;
		}

		// Connect to server.
		iResult = connect(this->ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			closesocket(this->ConnectSocket);
			this->ConnectSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}

	freeaddrinfo(result);

	if (this->ConnectSocket == INVALID_SOCKET) {
		printf("Unable to connect to server!\n");
		WSACleanup();
		return 1;
	}
}

bool Client::sendCommand(std::string command)
{
	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;
	// Send a buffer
	std::string out = command;
	out = out.append("_");
	out = out.append(this->username);
	const char *sendbuf = out.c_str();

	int iResult = send(this->ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
	if (iResult == SOCKET_ERROR) {
		printf("\nSend failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return false;
	}

	return true;
}


bool Client::clientClose()
{
	char recvbuf[DEFAULT_BUFLEN];
	int iResult;
	int recvbuflen = DEFAULT_BUFLEN;

	// shutdown the connection since no more data will be sent
	iResult = shutdown(this->ConnectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("\nShutdown failed with error: %d\n", WSAGetLastError());
		closesocket(this->ConnectSocket);
		WSACleanup();
		return false;
	}

	// Receive until the peer closes the connection
	do {

		iResult = recv(this->ConnectSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0) {
			printf("\nMessage received: %d\n", recvbuf);
		}
		else if (iResult == 0) {
			printf("\nConnection closed\n");
		}
		else{
			printf("\nrecv failed with error: %d\n", WSAGetLastError());
		}
	} while (iResult > 0);

	// cleanup
	closesocket(this->ConnectSocket);
	WSACleanup();

	return true;
}

bool Client::rec_msg() {
	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;

	int iResult = recv(this->ConnectSocket, recvbuf, recvbuflen, 0);
	std::string recvbuf2(recvbuf, iResult);
	if (iResult > 0) {
		printf("\nMessage received:\n%d\n", recvbuf2.c_str());
	}
	else if (iResult == 0) {
		printf("\nConnection closed\n");
	}
	else {
		printf("\nrecv failed with error: %d\n", WSAGetLastError());
	}
	return true;
}

bool Client::clientMain()
{
	std::string in = "";
	bool res = true;
	bool rec = false;

	while (in != "Q") {
		std::cout << "\nMove?\n";
		std::cin >> in;
		if (this->isIn(COMMANDS, in)) {
			printf("Sending command: %s\n", in.c_str());
			res = this->sendCommand(in);
			if (!res) {
				std::cout << "Error sending command: %c\n",in;
				return false;
			}
			rec_msg();
		}
	}
	std::cout << "\nBye bye!\n";

	return this->clientClose();
	return true;
}

bool Client::isIn(std::string arrayIn[], std::string value) {
	for (int i = 0; i < sizeof(arrayIn)+1; i++) {
		if (arrayIn[i] == value) {
			return true;
		}
	}
	return false;
}