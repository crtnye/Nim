// playNim.cpp
// This set of functions are used to actually play the game.
// Play starts with the function: playNim() which is defined below

#include "nim.h"
#include <WinSock2.h>
#include <iostream>
#include <string>

void initializeBoard( char board[10] )
{
	
}

void updateBoard( char board[10], int move, int Player)
{

}

void displayBoard( char board[10] )
{

}

int check4Win(char board[10])
{
	return 1;
}

int getMove(char board[10], int Player)
{
	return 1;
}

int playNim(SOCKET s, std::string serverName, std::string remoteIP, std::string remotePort, int localPlayer)
{
	// This function plays the game and returns the value: winner.  This value 
	// will be one of the following values: noWinner, CWinner, PWinner, TIE, ABORT
	int winner = noWinner;
	char board[10];
	int opponent;
	int move;
	bool myMove;

	if (localPlayer == PCLIENT) {
		std::cout << "Playing as X" << std::endl;
		opponent = PSERVER;
		myMove = true;
	} else {
		std::cout << "Playing as O" << std::endl;
		opponent = PCLIENT;
		myMove = false;
	}

	initializeBoard(board);
	displayBoard(board);

	while (winner == noWinner) {
		if (myMove) {
			// Get my move & display board
			move = getMove(board, localPlayer);
			std::cout << "Board after your move:" << std::endl;

			updateBoard(board,move,localPlayer);
			displayBoard(board);

			// Send move to opponent
/****			
	Task 1: "move" is an integer that was assigned a value (from 1 to 9) in the previous code segment.
	         Add code here to convert "move" to a null-terminated C-string and send it to your opponent at remoteIP using remotePort.
****/	
			char _move[2];
			_itoa_s(move, _move, 10);
			int numBytesSent = UDP_send(s, _move, strlen(_move) + 1, (char*)remoteIP.c_str(), (char*)remotePort.c_str());

		} else {
			std::cout << "Waiting for your opponent's move..." << std::endl << std::endl;
			//Get opponent's move & display board
			int status = wait(s,WAIT_TIME,0);
			if (status > 0) {
/****			
Task 2: (i) Insert code inside this IF statement that will accept a null-terminated C-string from your
		opponent that represents their move.  Convert that string to an integer and then
		(ii) call a function that will update the game board (see above) using your opponent's move, and
		(iii) call a function that will display the updated board on your screen.
****/			
				char moveStr[MAX_RECV_BUF];
				char remoteHost[v4AddressSize];
				char remotePort[v4AddressSize];
				int numBytesRecvd = UDP_recv(s, moveStr, MAX_RECV_BUF, remoteHost, remotePort);

				int move = atoi(moveStr);

				updateBoard(board, move, opponent);
				displayBoard(board);

			} else {
				winner = ABORT;
			}
		}
		myMove = !myMove;

		if (winner == ABORT) {
			std::cout << timestamp() << " - No response from opponent.  Aborting the game..." << std::endl;
		} else {
			winner = check4Win(board);
		}
		

	}

	return winner;
}