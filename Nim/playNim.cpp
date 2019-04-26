// playNim.cpp
// This set of functions are used to actually play the game.
// Play starts with the function: playNim() which is defined below

#include "nim.h"
#include <WinSock2.h>
#include <iostream>
#include <string>

//Returns the initial pile configuration 
std::string initializeBoard(int piles[])
{
	//TODO:
	//Initialize the game board
	//Calculate a random number of piles between 3 and 9 inclusive
	//Calculate a random number of rocks between 1 and 20 inclusive for each pile
	//Populate piles[] with the data as it is generated
	//Build the return string



	std::string temp;
	return temp;
}

void updateBoard(int piles[], std::string move, int Player)
{
	//TODO: Update the piles array to reflect the move
}

void displayBoard(int piles[])
{
	//TODO: Display the game board
}

int check4Win(int piles[])
{
	//TODO: Check to see if the game is over
	//Notify the player if the game is over and who won.
	return 0;
}

std::string getMove(const int piles[], int Player)
{
	//TODO:
	//Ask the player for their move
	//Ask for pile, then number of rocks to remove (make sure both are valid input)
	//return the move as a string
	std::string temp = "";
	return temp;
}

int playNim(SOCKET s, std::string serverName, std::string remoteIP, std::string remotePort, int localPlayer)
{
	// This function plays the game and returns the value: winner.  This value 
	// will be one of the following values: noWinner, CWinner, PWinner, ABORT
	int winner = noWinner;
	std::string initialBoardConfig; //Contains the initial mnnnnnnnn format
	int piles[9]; //Keeps track of the piles during gameplay. piles[0] contains the number of rocks the first pile has left.
	int opponent;
	std::string move;
	bool myMove;

	if (localPlayer == PCLIENT) {
		opponent = PSERVER;
		myMove = true;
	} else {
		opponent = PCLIENT;
		myMove = false;
		initialBoardConfig = initializeBoard(piles);

		//TODO: Send intialBoardConfig to the server using UDP_send
	}

	displayBoard(piles);

	while (winner == noWinner) {
		if (myMove) {
			// Get my move & display board
			move = getMove(piles, localPlayer);
			//TODO: Some checks on the move the player wants to make
			//Did the player forfeit?
			//Is the player just sending a comment?
			//Is the move they sent invalid?

			std::cout << "Board after your move:" << std::endl;

			updateBoard(piles,move,localPlayer);
			displayBoard(piles);

			// Send move to opponent

			char mutableMove[4];
			strcpy(mutableMove, move.c_str());

			int numBytesSent = UDP_send(s, mutableMove, strlen(mutableMove) + 1, (char*)remoteIP.c_str(), (char*)remotePort.c_str());

		} else {
			std::cout << "Waiting for your opponent's move..." << std::endl << std::endl;

			//Get opponent's move & display board
			int status = wait(s,WAIT_TIME,0);
			if (status > 0) {

				char moveStr[MAX_RECV_BUF];
				char remoteHost[v4AddressSize];
				char remotePort[v4AddressSize];
				int numBytesRecvd = UDP_recv(s, moveStr, MAX_RECV_BUF, remoteHost, remotePort);

				std::string move = moveStr;
				//TODO: Some checks on the move recieved (probably a function)
				//Did they forfeit?
				//Are they just sending a comment?
				//Is the move they sent invalid?



				updateBoard(piles, move, opponent);
				displayBoard(piles);

			} else {
				winner = ABORT;
			}
		}
		myMove = !myMove;

		if (winner == ABORT) {
			std::cout << timestamp() << " - No response from opponent.  Aborting the game..." << std::endl;
		} else {
			winner = check4Win(piles);
		}

	}
	return winner;
}