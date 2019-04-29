// server_main.cpp
//   This function serves as the "main" function for the server-side
#include "nim.h"
#include <iostream>
#include <string>
#include <WinSock2.h>

//int serverMain(int argc, char *argv[], std::string playerName)
int serverMain(int argc, char argv[], std::string playerName)
{
	SOCKET s;
	char buffer[MAX_RECV_BUF];
	std::string host;
	std::string port;
	char responseStr[MAX_SEND_BUF];

	s = passivesock(NIM_UDPPORT, "udp");

	std::cout << std::endl << "Waiting for a challenge..." << std::endl;
	

	bool finished = false;
	while (!finished) {
		int len = UDP_recv(s, buffer, MAX_RECV_BUF, (char*)host.c_str(), (char*)port.c_str());
		std::cout << timestamp() << " - Received: " << buffer << std::endl;
		if (strcmp(buffer, NIM_QUERY) == 0) {
			// Respond to name query
			strcpy_s(responseStr, NIM_NAME);
			strcat_s(responseStr, playerName.c_str());
			int bytesSent = UDP_send(s, responseStr, strlen(responseStr) + 1, (char*)host.c_str(), (char*)port.c_str());
			std::cout << timestamp() << " - Sending: " << responseStr << std::endl;
			wait(s, 10, 0);
		}
		else if (strncmp(buffer, NIM_CHALLENGE, strlen(NIM_CHALLENGE)) == 0) {
			// Received a challenge  
			char *startOfName = strstr(buffer, NIM_CHALLENGE);
			if (startOfName != NULL) {
				std::cout << std::endl << "You have been challenged by " << startOfName + strlen(NIM_CHALLENGE) << std::endl;
			}
			std::cout << "Would you like to accept the challenge? " << std::endl;
			char reply;
			std::cin >> reply;

			if (reply == 'y' || reply == 'Y') {
				int bytesSent = UDP_send(s, "YES", 5, (char*)host.c_str(), (char*)port.c_str());
				int status = wait(s, 2, 0);
				int len = UDP_recv(s, buffer, MAX_RECV_BUF, (char*)host.c_str(), (char*)port.c_str());
				if (status > 0 && strcmp(buffer, "GREAT!") == 0) {
					// Play the game.  You are the 'O' player
					int winner = playNim(s, (char*)playerName.c_str(), (char*)host.c_str(), (char*)port.c_str(), PSERVER);
					finished = true;
				}
				wait(s, 5, 0);

			}

			if (!finished) {
				char previousBuffer[MAX_RECV_BUF];		strcpy_s(previousBuffer, buffer);
				std::string previousHost;				previousHost = host;
				std::string previousPort;				previousPort = port;

				// Check for duplicate datagrams (can happen if broadcast enters from multiple ethernet connections)
				bool newDatagram = false;
				int status = wait(s, 1, 0);	// We'll wait a second to see if we receive another datagram
				while (!newDatagram && status > 0) {
					len = UDP_recv(s, buffer, MAX_RECV_BUF, (char*)host.c_str(), (char*)port.c_str());
					std::cout << timestamp() << " - Received: " << buffer << std::endl;
					if (strcmp(buffer, previousBuffer) == 0 &&		// If this datagram is identical to previous one, ignore it.
						host == previousHost &&
						port == previousPort) {
						status = wait(s, 1, 0);			// Wait another second (still more copies?)
					}
					else {
						newDatagram = true;		// if not identical to previous one, keep it!
					}
				}

				// If we waited one (or more seconds) and received no new datagrams, wait for one now.
				if (!newDatagram) {
					len = UDP_recv(s, buffer, MAX_RECV_BUF, (char*)host.c_str(), (char*)port.c_str());
					std::cout << timestamp() << " - Received: " << buffer << std::endl;
				}
			}
		}

	}
		closesocket(s);
	return 0;
}