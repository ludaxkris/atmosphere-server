#include "StdAfx.h"
#include ".\connection.h"
#using <mscorlib.dll>

#include <iostream>
using namespace std;

connection::connection(void)
{
	//init lock
	InitializeCriticalSection(&m_CriticalSectionSend); 
	InitializeCriticalSection(&m_CriticalSectionRecv); 
	debug = 0;
	stackTrace = 0;
}

connection::~connection(void)
{
	//free lock
	DeleteCriticalSection(&m_CriticalSectionSend); 
	DeleteCriticalSection(&m_CriticalSectionRecv); 
}
/**************************************************************************
	*					closeConnection
	*
	*	Closes the connection to the clientSocket
	*
	*
	**************************************************************************/
int connection::closeConnection(SOCKET* ClientSocket)
{
	//lock, only one should ever close the connection but just in case
	EnterCriticalSection(&m_CriticalSectionSend); 
	EnterCriticalSection(&m_CriticalSectionRecv); 
	
	// shutdown the connection since we're done
	//Nick de ug
	if(ClientSocket == NULL)
	{
		cerr<<"Found NULL Client UH OH....ARJAY.."<<endl;
		return 1;
	}

	iResult = shutdown( (*ClientSocket), SD_SEND);

	if (iResult == SOCKET_ERROR) {
		if(debug)
			printf("shutdown failed: %d\n", WSAGetLastError());
		closesocket( (*ClientSocket));

		
		//unlock
		LeaveCriticalSection(&m_CriticalSectionSend); 
		LeaveCriticalSection(&m_CriticalSectionRecv); 
		return 1;
	}
	
	closesocket( (*ClientSocket));
	
	if(debug)
		cerr<<"Closing SOCKET::"<<ClientSocket;


	//unlock
	LeaveCriticalSection(&m_CriticalSectionSend); 
	LeaveCriticalSection(&m_CriticalSectionRecv); 

	return 0;

}


	/**************************************************************************
	*					reciveDataFromClient
	*
	*	Recives the data from the client, ClientSocket and returns it (void*)
	*
	*
	**************************************************************************/
void* connection::reciveDataFromClient(SOCKET* ClientSocket, int* size, int* end)
{

	//lock, only one thread in each con should enter but just in case
	EnterCriticalSection(&m_CriticalSectionRecv); 
	if(stackTrace)
		cerr<<"connection::reciveDataFromClient START"<<endl;

	//recive first four bytes n find out how much to allocate on heap
	int bufferSize;

	bufferSize = 0;
	iResult = recv((*ClientSocket), (char *)(&bufferSize), sizeof(int), 0);
	if(debug)
		cerr<<"BUFFERSIZE:::"<<bufferSize<<endl;


	(*size) = bufferSize;
	char* buffer;
	if (iResult > 0) {

		if(debug)
		{
			printf("Bytes received: %d\n", iResult);
			printf("Size of next Message:: %d\n",bufferSize);
		}
//scratch playerID for now, secuirty isn't a priority <TO DO> increese secuirty 
//by implimenting the player id or session cookie...

		//recive the 5th byte for playerID
		char playerID = 0;
		iResult = recv((*ClientSocket), (char *)(&playerID), 1, 0);
		if(debug)
		{
			printf("Bytes received: %d\n", iResult);
			printf("PlayerID:: %d\n",playerID);
		}
		//create a buffer of the size that we need
		buffer = (char*)malloc(bufferSize);

		if(bufferSize)
			iResult = recv((*ClientSocket), buffer, bufferSize, 0);

		if(debug)
		{
			printf("Bytes received: %d\n", iResult);
			printf("MSG:: %s\n",buffer);
		}
	if(stackTrace)
		cerr<<"connection::reciveDataFromClient END"<<endl;
		//unlock

		(*end) = 0;
		LeaveCriticalSection(&m_CriticalSectionRecv); 
		
		return (void*)buffer;
	}
	else if (iResult == 0)
	{
		(*end) = 1;
		if(debug)
			printf("Connection closing...\n");
	
		//unlock
		if(stackTrace)
			cerr<<"connection::reciveDataFromClient END"<<endl;
		LeaveCriticalSection(&m_CriticalSectionRecv); 
		return (void*)0;
	}
	else  {
		if(debug)
			printf("recv failed: %d\n", WSAGetLastError());
		closesocket((*ClientSocket));


		//unlock
		if(stackTrace)
			cerr<<"connection::reciveDataFromClient END"<<endl;
		LeaveCriticalSection(&m_CriticalSectionRecv); 
		(*end) = 1;
		return (void*)0;
	}

}

//This sends data to a client we're connected to
//data is the data we want to send, dataSize is the size of the data we want to send
int connection::sendDataToClient(SOCKET* ClientSocket, void* data, int dataSize)
{

	//lock
	EnterCriticalSection(&m_CriticalSectionSend); 


	char command = (*((char*) data)) + '0';
	if(debug)
	{
		cerr<<"COMMAND::"<<command<<endl;
		if(data)
		cerr<<"data::"<<*((int*)data)<<endl;
	}

	if(stackTrace)
		cerr<<"connection::sendDataToClient START"<<endl;

	//send size
	if(dataSize > 0)
	{
	    iResult = send( (*ClientSocket), (char*)(&dataSize), sizeof(int), 0 );
	
		//send Data
		iResult = send( (*ClientSocket), (char*)data, dataSize, 0 );


		if (iResult == SOCKET_ERROR) {
			if(debug)
				printf("send failed: %d\n", WSAGetLastError());

			//unlock
			LeaveCriticalSection(&m_CriticalSectionSend); 
			return 1;
		}
	}
	
	if(debug)
		printf("sendDataToServer(void*, int):: Bytes Sent: %ld\n", iResult);
	
	if(stackTrace)
		cerr<<"connection::sendDataToClient END"<<endl;
	//unlock
	LeaveCriticalSection(&m_CriticalSectionSend); 
	return 0;
}

//This sends data to the server we're connected to
//data is the data we want to send, dataSize is the size of the data we want to send
//Command is the command that we want to send over
//the datasize is excluding the command
int connection::sendDataToClient(SOCKET* ClientSocket, void* data, int dataSize, char command)
{
	//lock
	EnterCriticalSection(&m_CriticalSectionSend); 
	if(stackTrace)
		cerr<<"connection::sendDataToClient START"<<endl;



	
	//set new size of data
	int newDataSize = dataSize+1;
	//allocate space for new data
	void* newData = malloc(newDataSize);

	//copy command to front of new data
	memcpy ( newData, (const void *)(&command), 1 );

	//copy rest of the data we want to send to the back of command
	//cast newData to a char* because we need to add 1 byte to it

	if(dataSize > 0)
		memcpy ( ( ((char*)newData) +1), (const void *)(data), dataSize );




	//send size
    iResult = send( (*ClientSocket) , (char*)(&newDataSize), sizeof(int), 0 );
	
	//send Data
	iResult = send( (*ClientSocket), (char*)newData, newDataSize, 0 );


    if (iResult == SOCKET_ERROR) {
		if(debug)
	        printf("send failed: %d\n", WSAGetLastError());
        return 1;
    }

	if(debug)
		printf("sendDataToServer(void*, int):: Bytes Sent: %ld\n", iResult);



	//clean up stuff we sent
	free (newData);

	if(debug)
		printf("sendDataToServer(void*, int, char):: Bytes Sent: %ld\n", iResult);
	
	if(stackTrace)
		cerr<<"connection::sendDataToClient END"<<endl;
	//unlock
	LeaveCriticalSection(&m_CriticalSectionSend); 

	return 0;
}

