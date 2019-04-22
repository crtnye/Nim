// clientMain.cpp
//   This function serves as the "main" function for the client-side
#include "TicTacToe.h"
#include <iostream>
#include <string>
#include <WinSock2.h>

int clientMain(int argc, char *argv[], std::string playerName)
{
	std::string host;
	std::string port;
	ServerStruct serverArray[MAX_SERVERS];

	SOCKET s = connectsock("","","udp");	// Create a socket  (Don't need to designate a host or port for UDP)

	// Find all TicTacToe servers on our subnet
	std::cout << std::endl << "Looking for TicTacToe servers ... " << std::endl;

	char broadcastAddress[v4AddressSize];
	char myIPAddress[v4AddressSize];
	int status = getIPAddressInfo(myIPAddress, broadcastAddress);
	if (status != 0) {
		std::cerr << "No interface found that supports broadcasting." << std::endl;
		return -1;
	}
	int numServers = getServers(s, broadcastAddress, TicTacToe_UDPPORT, serverArray);

	if (numServers == 0) {
		std::cout << std::endl << "Sorry.  No TicTacToe servers were found.  Try again later." << std::endl << std::endl;
	} else {
		// Display the list of servers found
		std::cout << std::endl << "Found TicTacToe server";
		if (numServers == 1) {
			std::cout << ":" << "  " << serverArray[0].name << std::endl;
		} else {
			std::cout << "s:" << std::endl;
			for (int i=0; i<numServers; i++) {
				std::cout << "  " << i+1 << " - " << serverArray[i].name << std::endl;
			}
			std::cout << std::endl << "  " << numServers+1 << " - QUIT" << std::endl;
		}
		std::cout << std::endl;

		// Allow user to select someone to challenge
		int answer = 0;
		std::string answerStr;
		if (numServers == 1) {
			std::cout << "Do you want to challenge " << serverArray[0].name << "? ";
			std::getline(std::cin, answerStr);
			if (answerStr[0] == 'y' || answerStr[0] == 'Y') answer = 1;
		} else if (numServers > 1) {
			std::cout << "Who would you like to challenge (1-" << numServers+1 << ")? ";
			std::getline(std::cin,answerStr);
			answer = atoi(answerStr.c_str());
			if (answer > numServers) answer = 0;
		}
			
		if (answer >= 1 && answer <= numServers) {
			// Extract the opponent's info from the server[] array
			std::string serverName;
			serverName = serverArray[answer-1].name;		// Adjust for 0-based array
			host = serverArray[answer-1].host;
			port = serverArray[answer-1].port;

			// Append playerName to the TicTacToe_CHALLENGE string & send a challenge to host:port
			char buffer[MAX_SEND_BUF];
			strcpy_s(buffer,TicTacToe_CHALLENGE);
			strcat_s(buffer,playerName.c_str());
			int len = UDP_send(s, buffer, strlen(buffer)+1,(char*)host.c_str(), (char*)port.c_str());


			// Play the game.  You are the 'X' player
			int winner = playTicTacToe(s, serverName, host, port, X_PLAYER);
		}
	}

	closesocket(s);
	return 0;
}