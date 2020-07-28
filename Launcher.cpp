#include <thread>

#include "Client.h"

#include <iostream>
#include <string>

int main() {

	std::string username_in, password_in = "";
	username_in = "jess";
	password_in = "123";

	std::cout << "Username?\n";
	printf("%s\n", username_in.c_str());
	std::cout << "Password?\n";
	printf("%s\n", password_in.c_str());

	std::cout << "Launching client...";
	Client gameClient = Client(username_in);
	gameClient.clientMain();
}