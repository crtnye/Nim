// clientMain.cpp
//   This function serves as the "main" function for the client-side
#include "nim.h"
#include <iostream>
#include <string>
#include <WinSock2.h>

int clientMain(int argc, char *argv[], std::string playerName)
{
	std::string host;
	std::string port;
	ServerStruct serverArray[MAX_SERVERS];

	//Create a client socket
	SOCKET s = connectsock("", "", "udp");

	// Find all Nim servers on our subnet
	//std::cout << std::endl << "Looking for Nim servers ... " << std::endl;

	char broadcastAddress[v4AddressSize];
	char myIPAddress[v4AddressSize];

	int status = getIPAddressInfo(myIPAddress, broadcastAddress);
	if (status != 0) {
		std::cerr << "No interface found that supports broadcasting." << std::endl;
		return -1;
	}
	int numServers = getServers(s, broadcastAddress, NIM_UDPPORT, serverArray);

	if (numServers == 0) {
		std::cout << std::endl << "Sorry.  No NIM servers were found.  Try again later." << std::endl << std::endl;
	} 
	else {
		// Display the list of servers found
		std::cout << std::endl << "Found NIM server";
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

		bool opponentSelected = false;
		while (!opponentSelected){
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
				serverName = serverArray[answer - 1].name;		// Adjust for 0-based array
				host = serverArray[answer - 1].host;
				port = serverArray[answer - 1].port;

				// Append playerName to the NIM_CHALLENGE string & send a challenge to host:port
				char buffer[MAX_SEND_BUF];
				strcpy_s(buffer, NIM_CHALLENGE);
				strcat_s(buffer, playerName.c_str());
				int len = UDP_send(s, buffer, strlen(buffer) + 1, (char*)host.c_str(), (char*)port.c_str());

				int status = wait(s, 30, 0);

				//Recieve the reply from the server you challenged.
				char recvBuffer[MAX_RECV_BUF];
				int recvLen = UDP_recv(s, recvBuffer, strlen(recvBuffer) + 1, (char*)host.c_str(), (char*)port.c_str());

				if (status > 0 && strcmp(recvBuffer, "YES") == 0) {
					char confirm[MAX_SEND_BUF] = "GREAT!";
					int len = UDP_send(s, confirm, strlen(confirm) + 1, (char*)host.c_str(), (char*)port.c_str());

					opponentSelected = true;
					// Play the game.  You are the 'X' player
					int winner = playNim(s, serverName, host, port, PCLIENT);
				}
				else {
					//The answer is either "NO", invalid input, or the timer timed out.
					//Notify the user and allow them to challenge someone else or quit.
					std::cout << "I'm sorry, that player does not want to play with you." << std::endl;
					answer = 0;
				}
			}
		}
	}
	closesocket(s);
	return 0;
}