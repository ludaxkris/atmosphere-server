#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>

class connection
{
		CRITICAL_SECTION m_CriticalSectionSend; 
		CRITICAL_SECTION m_CriticalSectionRecv; 

		int iResult, iSendResult;
		int debug;

public:
		int stackTrace;


	/**************************************************************************
	*					reciveDataFromClient
	*
	*	Description: This function recives the information from the socket
	*				 ClientSocket and passes it back to the caller
	*
	*				Uses: we may use this for our lobby so I made it public
	*
	*				size: the size of the data returned
	*
	*				end: if the client closed the connection, 0 = no,
	*					1 = end connectoin
	*
	*
	**************************************************************************/
	void* reciveDataFromClient(SOCKET* ClientSocket, int* size, int* end);
	/**************************************************************************
	*					closeConnection
	*
	*	Description: This function closes the connection to the socket
	*				ClientSocket.
	*
	*	ClientSocket: the socket in which you want to close
	*	
	*	Returns: 1 for error and 0 if everything went well
	*
	**************************************************************************/
	int closeConnection(SOCKET* ClientSocket);

	/**************************************************************************
	*					sendDataToClient
	*
	*	Description: This function sends the information to the socket
	*				 ClientSocket and passes number of bytes sent
	*
	*
	*				ClientSocket: the socket to the client that we want to
	*							send data to.
	*		
	*				data:		The data we want to send to the client
	*							We do not free/delte the data
	*
	*				dataSize:	the size of the data we want to send
	*
	*				Uses: we may use this for our lobby so I made it public
	*
	*				return: 0 if okay and 1 if send failed
	*
	**************************************************************************/
	int sendDataToClient(SOCKET* ClientSocket, void* data, int dataSize);

	int sendDataToClient(SOCKET* ClientSocket, void* data, int dataSize, char command);

	connection(void);
	~connection(void);
};
