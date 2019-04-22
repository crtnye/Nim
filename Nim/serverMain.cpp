// server_main.cpp
//   This function serves as the "main" function for the server-side
#include "TicTacToe.h"
#include <iostream>
#include <string>
#include <WinSock2.h>

int serverMain(int argc, char *argv[], std::string playerName)
{
	SOCKET s;
	char buffer[MAX_RECV_BUF];
	std::string host;
	std::string port;
	char responseStr[MAX_SEND_BUF];
	
	s = passivesock(TicTacToe_UDPPORT,"udp");

	std::cout << std::endl << "Waiting for a challenge..." << std::endl;
	int len = UDP_recv(s, buffer, MAX_RECV_BUF, (char*)host.c_str() , (char*)port.c_str());
	std::cout << timestamp() << " - Received: " << buffer << std::endl;

	bool finished = false;
	while (!finished) {
		if ( strcmp(buffer,TicTacToe_QUERY) == 0) {
			// Respond to name query
			strcpy_s(responseStr,TicTacToe_NAME);
			strcat_s(responseStr,playerName.c_str());
			UDP_send(s, responseStr, strlen(responseStr)+1, (char*)host.c_str(), (char*)port.c_str());
			std::cout << timestamp() << " - Sending: " << responseStr << std::endl;

		} else if ( strncmp(buffer,TicTacToe_CHALLENGE,strlen(TicTacToe_CHALLENGE)) == 0) {
			// Received a challenge  
			char *startOfName = strstr(buffer,TicTacToe_CHALLENGE);
			if (startOfName != NULL) {
				std::cout << std::endl << "You have been challenged by " << startOfName+strlen(TicTacToe_CHALLENGE) << std::endl;
			}
			
			// Play the game.  You are the 'O' player
			int winner = playTicTacToe(s, (char*) playerName.c_str(), (char*)host.c_str(), (char*)port.c_str(), O_PLAYER);
			finished = true;
		}

		if (!finished) {
			char previousBuffer[MAX_RECV_BUF];		strcpy_s(previousBuffer,buffer);
			std::string previousHost;				previousHost = host;
			std::string previousPort;				previousPort = port;

			// Check for duplicate datagrams (can happen if broadcast enters from multiple ethernet connections)
			bool newDatagram = false;
			int status = wait(s,1,0);	// We'll wait a second to see if we receive another datagram
			while (!newDatagram && status > 0) {
				len = UDP_recv(s, buffer, MAX_RECV_BUF, (char*)host.c_str(), (char*)port.c_str());
				std::cout << timestamp() << " - Received: " << buffer << std::endl;
				if (strcmp(buffer,previousBuffer)==0 &&		// If this datagram is identical to previous one, ignore it.
					host == previousHost && 
					port == previousPort) {
						status = wait(s,1,0);			// Wait another second (still more copies?)
				} else {
					newDatagram = true;		// if not identical to previous one, keep it!
				}
			}

			// If we waited one (or more seconds) and received no new datagrams, wait for one now.
			if (!newDatagram ) {
				len = UDP_recv(s, buffer, MAX_RECV_BUF, (char*)host.c_str(), (char*)port.c_str());
				std::cout << timestamp() << " - Received: " << buffer << std::endl;
			}
		}
	}
	closesocket(s);

	return 0;
}