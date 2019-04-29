// playNim.cpp
// This set of functions are used to actually play the game.
// Play starts with the function: playNim() which is defined below

#include "nim.h"
#include <WinSock2.h>
#include <iomanip>
#include <array>
#include <iostream>
#include <sstream>
#include <string>
using namespace std;

//Returns the initial pile configuration 
string initializeBoard(int piles[])
{
	//TODO:
	//Initialize the game board
	//Calculate a random number of piles between 3 and 9 inclusive
	//Calculate a random number of rocks between 1 and 20 inclusive for each pile
	//Populate piles[] with the data as it is generated
	//Build the return string
	srand(time(0));
	int maxPiles = rand() % 9 + 3;
	char t[255];
	itoa(maxPiles, t, 10);
	string temp;
	temp.append(t);

	for (int i = 0; i < maxPiles; i++) {
		piles[i] = rand() % 20 + 1;
		itoa(piles[i], t, 10);
		if (piles[i] < 10) {
			temp.append("0");
			temp.append(t);
		}
		else {
			temp.append(t);
		}
	}
	
	return temp;
}

void updateBoard(int piles[], string move, int Player)
{
	//TODO: Update the piles array to reflect the move
	istringstream iss(move.substr(0,1));
	int p;
	iss >> p;

	int r;
	istringstream iss2(move.substr(1, 2));
	iss2 >> r;

	piles[p] -= r;

}

void displayBoard(int piles[])
{
	//TODO: Display the game board
	for (int i = 0; i < sizeof(piles); i++) {
		cout << "PILE " << i << ": ";
		for (int f = 0; f < piles[i]; i++) {
			cout << "*";
		}
		
		cout << right << setw(12) << "PILE " << i << ": " << piles[i] << endl;
	}
}

int check4Win(int piles[])
{
	//TODO: Check to see if the game is over
	//Notify the player if the game is over and who won.
	return 0;
}

string getMove(const int piles[], int Player)
{
	//TODO:
	//Ask the player for their move
	//Ask for pile, then number of rocks to remove (make sure both are valid input)
	//return the move as a string
	string temp = "";
	return temp;
}

int playNim(SOCKET s, string serverName, string remoteIP, string remotePort, int localPlayer)
{
	// This function plays the game and returns the value: winner.  This value 
	// will be one of the following values: noWinner, CWinner, PWinner, ABORT
	int winner = noWinner;
	string initialBoardConfig; //Contains the initial mnnnnnnnn format
	int piles[9]; //Keeps track of the piles during gameplay. piles[0] contains the number of rocks the first pile has left.
	int opponent;
	string move;
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

			cout << "Board after your move:" << endl;

			updateBoard(piles,move,localPlayer);
			displayBoard(piles);

			// Send move to opponent

			char mutableMove[4];
			strcpy(mutableMove, move.c_str());

			int numBytesSent = UDP_send(s, mutableMove, strlen(mutableMove) + 1, (char*)remoteIP.c_str(), (char*)remotePort.c_str());

		} else {
			cout << "Waiting for your opponent's move..." << endl << endl;

			//Get opponent's move & display board
			int status = wait(s,WAIT_TIME,0);
			if (status > 0) {

				char moveStr[MAX_RECV_BUF];
				char remoteHost[v4AddressSize];
				char remotePort[v4AddressSize];
				int numBytesRecvd = UDP_recv(s, moveStr, MAX_RECV_BUF, remoteHost, remotePort);

				string move = moveStr;
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
			cout << timestamp() << " - No response from opponent.  Aborting the game..." << endl;
		} else {
			winner = check4Win(piles);
		}

	}
	return winner;
}