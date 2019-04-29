#pragma once
// nim.h    Header file for Nim project
#include <winsock2.h>
#include <string>
using namespace std;

#define NIM_QUERY	"Who?"
#define NIM_NAME	"Name="
#define NIM_CHALLENGE	"Player="

static char NIM_UDPPORT[] = "29333";

const int v4AddressSize = 16;
const int portNumberSize = 10;
const int WAIT_TIME = 120;
const int MAX_SEND_BUF = 2048;
const int MAX_RECV_BUF = 2048;
const int MAX_SERVERS = 100;
const int MAX_NAME = 100;
const int ABORT = -1;
const int PCLIENT = 1;
const int PSERVER = 2;
const int noWinner = 0;
const int CWinner = 1;
const int PWinner = 2;

struct ServerStruct {
	string name;
	string host;
	string port;
};

struct Piles {
	int pile[9];
	char board[255];
	int numPiles;
};

int playNim(SOCKET, string, string, string, int);
SOCKET connectsock(const char*, const char*, const char*);
SOCKET passivesock(const char*, const char*);
int UDP_recv(SOCKET, char*, int, char*, char*);
int UDP_send(SOCKET, char*, int, char*, char*);
int wait(SOCKET, int, int);
char* timestamp();
int getServers(SOCKET, char*, char*, ServerStruct[]);
int serverMain(int, char[], string);
int clientMain(int, char*, string);
int getIPAddressInfo(char*, char*);