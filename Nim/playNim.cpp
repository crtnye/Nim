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
string initializeBoard(Piles &piles)
{
	srand(time(0));
	int maxPiles = rand() % 9 + 3;
	piles.numPiles = maxPiles;
	char t[3];
	itoa(maxPiles, t, 10);
	strcat(piles.board, t);

	for (int i = 0; i < maxPiles; i++) {
		//piles.pile[i] = rand() % 20 + 1;
		//int x = piles.pile[i];
		int x = rand() % 20 + 1;
		piles.pile[i] = x;
		itoa(x, t, 10);
		if (piles.pile[i] < 10) {

			strcat(piles.board, "0");
			strcat(piles.board, t);
		}
		else {
			strcat(piles.board, t);
		}
	}
	return piles.board;
}

void updateBoard(Piles &piles, string move, int Player)
{
	//TODO: Update the piles array to reflect the move
	istringstream iss(move.substr(0,1));
	int p;
	iss >> p;

	int r;
	istringstream iss2(move.substr(1, 2));
	iss2 >> r;

	piles.pile[p] -= r;

}

void displayBoard(Piles &piles)
{
	//TODO: Display the game board
	for (int i = 0; i < piles.numPiles; i++) {
		cout << "PILE " << i + 1 << ": ";
		for (int f = 0; f < piles.pile[i]; f++) {
			cout << "*";
		}
		cout << right << setw(12) << "PILE " << i + 1 << ": " << piles.pile[i] << endl;
	}
}

int check4Win(Piles &piles)
{
	//TODO: Check to see if the game is over
	//Notify the player if the game is over and who won.
	return 0;
}

char* getMove(Piles &piles, int Player)
{
	char message[MAX_SEND_BUF];

	cout << "It's your turn!" << endl;
	bool validCommand = false;

	while (!validCommand) {
		char command[36];

		cout << "Enter first letter of one of the following commands (C,F, or R)."
			<< "Command(Chat, Forfeit, Remove - rocks) ? ";
		cin >> command;


		if (command[0] == 'C') {
			validCommand = true;
			char comment[80];
			comment[0] = 'C';
			char input[79];
			cout << endl << "Comment?";
			cin >> input;
			strcat(comment, input);
			strcpy(message, comment);
		}
		else if (command[0] == 'F') {
			validCommand = true;
			cout << endl << "You forfeited, you lose." << endl;
			strcpy(message, (char*)'F');
		}
		else if (command[0] == 'R') {
			validCommand = true;
			int selectedPile;
			int numRocks;
			cout << "Which pile? ";
			cin >> selectedPile;
			cout << endl << "How many rocks?";
			cin >> numRocks;

			while (selectedPile < 1 || selectedPile > piles.numPiles || piles.pile[selectedPile] - numRocks < 0) {
				cout << "I'm sorry, you selected an invalid move. Please try again." << endl;

				cout << "Which pile? ";
				cin >> selectedPile;
				cout << endl << "How many rocks?";
				cin >> numRocks;
			}
			//put the move into message
			strcpy(message, (char*)selectedPile);
			if (numRocks < 10) {
				strcat(message, "0");
			}
			strcat(message, (char*)numRocks);
		}
		else {
			cout << "Invalid command." << endl;
		}
	}
	return message;
}

int playNim(SOCKET s, string serverName, string remoteIP, string remotePort, int localPlayer)
{
	// This function plays the game and returns the value: winner.  This value
	// will be one of the following values: noWinner, CWinner, PWinner, ABORT
	int winner = noWinner;
	string initialBoardConfig; //Contains the initial mnnnnnnnn format
	Piles piles; //Keeps track of the piles during gameplay. piles[0] contains the number of rocks the first pile has left.
	int opponent;
	bool myMove;

	if (localPlayer == PCLIENT) {
		opponent = PSERVER;
		myMove = true;
	} else {
		opponent = PCLIENT;
		myMove = false;
		initialBoardConfig = initializeBoard(piles);

		int numBytesSent = UDP_send(s, piles.board, strlen(piles.board) + 1, (char*)remoteIP.c_str(), (char*)remotePort.c_str());
	}

	displayBoard(piles);

	while (winner == noWinner) {
		if (myMove) {
			// Get my move & display board
			char *move = getMove(piles, localPlayer);

			int numBytesSent = UDP_send(s, move, strlen(move) + 1, (char*)remoteIP.c_str(), (char*)remotePort.c_str());

			if (move[0] == 'C') {
				cout << "Your comment has been sent." << endl;
			}
			else if (move[0] == 'F') {
				winner = opponent;
			}
			else {
				cout << "Board after your move:" << endl;

				updateBoard(piles, move, localPlayer);
				displayBoard(piles);

			}
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
