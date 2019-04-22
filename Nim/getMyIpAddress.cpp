#include <iostream>
#include <WS2tcpip.h>
const int  MAXHOSTNAME = 1024;
/*-----------------------------------------------------------
 * Function: getMyIPAddress()
 *
 * Description:
 *  Get the Local IP address using the following algorithm:
 *    - get local hostname with gethostname()
 *    - attempt to resolve local hostname using getaddrinfo()
 *
 *  Upon completion, this function returns a status integer
 *		If the function is successful, status == 0 and the parameter myIPAddress[]
 *		   will contain a pointer to a null-terminated, dotted-decimal string
 *		If there is a problem, the returned status value will be -1.
 */
int getMyIPAddress(char myIPAddress[], int maxSize) {
	// Parameter List:
	// [OUT] myIPAddress[] - null-terminated, dotted-decimal char array
	// [IN]  maxSize       - maximum number of bytes that can be stored in myIPAddress

	// Return Value : Completion stauts.  status == 0 indicates success

	// Note: This function requires the <iostream> and <WS2tcpip.h> header files
	//		as well as the ws2_32.lib library file

	int status = 0;
	char szLclHost[MAXHOSTNAME];

	// Get the local hostname
	int nRet = gethostname(szLclHost, MAXHOSTNAME);
	if (nRet != SOCKET_ERROR) {
		// Resolve hostname for local address
		struct addrinfo *result = NULL;
		DWORD dwRetval = getaddrinfo(szLclHost, 0, 0, &result);
		if (dwRetval == 0) {
			struct sockaddr_in *sockaddr_ipv4;
			sockaddr_ipv4 = (struct sockaddr_in *) result->ai_addr;
			struct in_addr stIPAddr;
			stIPAddr = sockaddr_ipv4->sin_addr;
			char buffer[MAXHOSTNAME] = "";
			if (inet_ntop(AF_INET, &stIPAddr, buffer, sizeof(buffer)) == NULL) {
				std::cout << "Problem with inet_ntop() function in getMyIPAddress()." << std::endl;
				status = -1;
			}
			strcpy_s(myIPAddress, maxSize, buffer);
		}
		else {
			std::cout << "Problem with getaddrinfo() function in getMyIPAddress()." << std::endl;
			status = -1;
		}
	}
	else {
		std::cout << "Problem with gethostname() function in getMyIPAddress()." << std::endl;
		status = -1;
	}

	return status;
} /* getMyIPAddress() */