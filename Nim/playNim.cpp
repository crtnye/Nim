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
	int maxPiles;
	srand(time(0));
		maxPiles = (rand() % 9) + 3;
		if (maxPiles > 9 || maxPiles < 3) {
			maxPiles = 5;
		}
	piles.numPiles = maxPiles;
	char t[3];
	itoa(maxPiles, t, 10);
	strcat(piles.board, t);

	for (int i = 0; i < maxPiles; i++) {
		//piles.pile[i] = rand() % 20 + 1;
		//int x = piles.pile[i];
		int x;
				x = (rand() % 20) + 1;
				if (x < 1 || x > 20) {
					x = 5;
				}
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

void populateBoard(Piles &piles, char boardConfig[]) {
	strcpy(piles.board, boardConfig);

	string board(boardConfig);
	istringstream iss(board.substr(0, 1));
	int p;
	iss >> p;

	piles.numPiles = p;

	//istringstream iss2(board.substr(1, 2));

	int startIndex = 1;
	for (int i = 0; i < p; i++) {
		int r;
		istringstream iss2(board.substr(startIndex, 2));
		iss2 >> r;

		piles.pile[i] = r;
		startIndex += 2;
	}
}

void updateBoard(Piles &piles, string move, int Player)
{
	//Update the piles array to reflect the move
	istringstream iss(move.substr(0,1));
	int p;
	iss >> p;

	int r;
	istringstream iss2(move.substr(1, 2));
	iss2 >> r;

	piles.pile[p-1] -= r;

}

void displayBoard(Piles &piles)
{
	//Display the game board
	for (int i = 0; i < piles.numPiles; i++) {
		cout << "PILE " << i + 1 << ": ";
		for (int f = 0; f < piles.pile[i]; f++) {
			cout << "*";
		}
		cout << right << setw(12) << "PILE " << i + 1 << ": " << piles.pile[i] << endl;
	}
}

int check4Win(Piles &piles, int localPlayer, int opponent)
{
	//Check to see if the game is over
	//Notify the player if the game is over and who won.
	//const int PCLIENT = 1;
	//const int PSERVER = 2;


	int winner;
	int totalRocks = 0;

	for (int i = 0; i < piles.numPiles; i++) {
		totalRocks += piles.pile[i];
	}

	if (totalRocks == 1) {
		winner = CWinner;
	}
	else {
		winner = noWinner;
	}

	//if (totalRocks == 1) {
	//	if (localPlayer == PCLIENT) {
	//		winner = 2;
	//	}
	//	else {
	//		winner = 1;
	//	}
	//}
	//else if (totalRocks == 0) {
	//	winner = localPlayer;
	//}
	//else {
	//	winner = noWinner;
	//}
	return winner;
}

void getMove(Piles &piles, int Player, char move[])
{

	cout << "It's your turn!" << endl;
	bool validCommand = false;

	while (!validCommand) {
		char command[36];

		cout << "Enter first letter of one of the following commands (C,F, or R)."
			<< "Command(Chat, Forfeit, Remove - rocks) ? ";
		cin >> command;


		if (command[0] == 'C' || command[0] == 'c') {
			validCommand = true;
			char comment[80];
			comment[0] = 'C';
			char input[79];
			cout << endl << "Comment?";
			cin >> input;
			strcat(comment, input);
			strcpy(move, comment);
		}
		else if (command[0] == 'F' || command[0] == 'f') {
			validCommand = true;
			cout << endl << "You forfeited, you lose." << endl;
			strcpy(move, (char*)'F');
		}
		else if (command[0] == 'R' || command[0] == 'r') {
			validCommand = true;
			int selectedPile;
			int numRocks;
			cout << "Which pile? ";
			cin >> selectedPile;
			cout << endl << "How many rocks?";
			cin >> numRocks;

			while (selectedPile < 1 || selectedPile-1 > piles.numPiles || piles.pile[selectedPile-1] - numRocks < 0) {
				cout << "I'm sorry, you selected an invalid move. Please try again." << endl;

				cout << "Which pile? ";
				cin >> selectedPile;
				cout << endl << "How many rocks?";
				cin >> numRocks;
			}
			//put the move into message
			char p[2];
			itoa(selectedPile, p, 10);
			strcpy(move, p);

			if (numRocks < 10) {
				strcat(move, "0");
			}

			char r[3];
			itoa(numRocks, r, 10);
			strcat(move, r);
		}
		else {
			cout << "Invalid command." << endl;
		}
	}
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
		char recvBuffer[MAX_RECV_BUF];

		int numBytesRecvd = UDP_recv(s, recvBuffer, MAX_RECV_BUF, (char*)remoteIP.c_str(), (char*)remotePort.c_str());
		//strcpy(piles.board, recvBuffer);
		populateBoard(piles, recvBuffer);

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
			char move[MAX_SEND_BUF];
			getMove(piles, localPlayer, move);
			move[strlen(move)] = '\0';

			int numBytesSent = UDP_send(s, move, strlen(move) + 1, (char*)remoteIP.c_str(), (char*)remotePort.c_str());

			if (move[0] == 'C') {
				cout << "Your comment has been sent." << endl;
			}
			else if (move[0] == 'F') {
				winner = opponent;
			}
			else {
				cout << "Board after your move:" << endl;

				//Update the board to reflect the move I just made
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

				char flag = moveStr[0];

				if (flag == 'C') {
					//sending a comment
					char comment[80];
					//Eliminate the leading 'C' from the string
					//I couldn't find a better way to do it
					for (int i = 0; i < strlen(moveStr); i++) {
						comment[i] = moveStr[i + 1];
					}

					cout << "[CHAT MESSAGE]: " << comment << endl;

				}
				else if (flag == 'F') {
					//if the other player forfeited
					winner = localPlayer;
					cout << "You won by default!" << endl;
				}
				else if (1 <= (flag - '0') <= 9){
					//if they sent a potentially valid move
					int selectedPile;
					int numRocks;
					string move(moveStr);
					
					istringstream iss(move.substr(0, 1));
					iss >> selectedPile;

					istringstream iss2(move.substr(1, 2));
					iss2 >> numRocks;

					//if the other player sent you an invalid move
					if (selectedPile < 1 || selectedPile - 1 > piles.numPiles || piles.pile[selectedPile - 1] - numRocks < 0) {
						winner = localPlayer;
						cout << "You won by default!" << endl;
					}
					else{
						cout << "Your opponent just removed " << numRocks << " from pile " << selectedPile << endl;
						//Update the board to reflect the move you just recieved
						updateBoard(piles, move, opponent);
						displayBoard(piles);
					}
				}
				else {
					winner = localPlayer;
					cout << "You won by default!" << endl;
				}
			} else {
				winner = ABORT;
			}
		}
		myMove = !myMove;

		if (winner == ABORT) {
			cout << timestamp() << " - No response from opponent.  Aborting the game..." << endl;
		} else {
			winner = check4Win(piles, localPlayer, opponent);
		}
	}
	return winner;
}
