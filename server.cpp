#include "StdAfx.h"
#include ".\server.h"


#include "stdafx.h"
#include <iostream>


using namespace System;
using namespace System::Threading;
using namespace std;
using namespace System::Timers;














server::~server(void)
{
	delete GH;

	//delete the cages inside the cage array
	for(int i = 0; i<currentCage; i++)
	{
		delete cageArray[i];
	}

	//delete the cage array
	delete [] cageArray;

}






server::server(void)
{
	//pointer to the game handler.  this is passed to the clients so they can join and leave
	//games.
	GH = new GameHandler();

	//set debug messages on or not.
	debug = 0;

	//what the current cage is
	currentCage = 0;
	
	//allocate an array of cageArray pointers for use later.
	cageArray = new ArjayThreadCage* [MAXCAGE];


}

//returns 1 if errored out or
//returns 0 if okay
//takes in the port you want to accept the connection on
//this will fill up the client sockets array
int server::openAndAcceptNewClients(char* port)
{

	//somonoe just turned on the server, wait for the command to stop the server.
	isOn = 1;

	//while the nothing messes with the server.
	while(isOn)
	{
		//create dynamic heap space for the ListenSocket
		ListenSocket = (SOCKET*) malloc(sizeof(SOCKET));
		(*ListenSocket) = INVALID_SOCKET;


		addrinfo *result = NULL;



		// Initialize Winsock
		iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
		if (iResult != 0) 
		{
			printf("WSAStartup failed: %d\n", iResult);
			return 1;
		}

		ZeroMemory(&hints, sizeof(hints));
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;
		hints.ai_flags = AI_PASSIVE;

		// Resolve the server address and port
		iResult = getaddrinfo(NULL, port, &hints, &result);
		if ( iResult != 0 ) 
		{
			printf("getaddrinfo failed: %d\n", iResult);
			WSACleanup();
			return 1;
		}

		// Create a SOCKET for connecting to server
		(*ListenSocket) = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
		if ( (*ListenSocket) == INVALID_SOCKET) 
		{
			printf("socket failed: %ld\n", WSAGetLastError());
			freeaddrinfo(result);
			WSACleanup();
			return 1;
		}

		// Setup the TCP listening socket
		iResult = bind( (*ListenSocket), result->ai_addr, (int)result->ai_addrlen);
		if (iResult == SOCKET_ERROR) 
		{
			printf("bind failed: %d\n", WSAGetLastError());
			freeaddrinfo(result);
			closesocket( (*ListenSocket));
			WSACleanup();
			return 1;
		}
cerr<<"READY FOR CLIENTS"<<endl;
		freeaddrinfo(result);
		iResult = listen( (*ListenSocket), SOMAXCONN);
		if (iResult == SOCKET_ERROR) 
		{
			printf("listen failed: %d\n", WSAGetLastError());
			closesocket( (*ListenSocket));
			WSACleanup();
			return 1;
		}

		SOCKET* ClientSocket = (SOCKET*)malloc(sizeof(SOCKET));
		(*ClientSocket) = INVALID_SOCKET;
		(*ClientSocket) = accept((*ListenSocket), NULL, NULL);
		if ( (*ClientSocket) == INVALID_SOCKET) 
		{
			printf("accept failed: %d\n", WSAGetLastError());
			closesocket((*ListenSocket));
			WSACleanup();
			return 1;
		}

	


		// Accept a client socket and continually call reciveDataFromClient to get data
		//then call func on data over and over again until the client closes connection
		//on a new thread.

		//we have the client socket for the client, make a ArjayThreadCage
		//with a new thread to serve that client.
		spawnPlayerThread(ClientSocket);

		closesocket((*ListenSocket));
		delete ListenSocket;

	}

	return 0;
}

void server::spawnPlayerThread(SOCKET* clientSocket)
{
	//create container for information to be sent to thread.
	ThreadInfo* passInfo = new ThreadInfo(clientSocket, GH);	


	//spawn new thread
	int curThreadSpot = getNextEmptyThreadSpot();

	DWORD threadID;

	//create and save cage for the thread to be placed in
	cageArray[curThreadSpot] = new ArjayThreadCage(passInfo);
	
	//create thread on the function inside the new cage...
	//create and cage the new thread, pass it the info it needs to run
	threadArray[curThreadSpot] = CreateThread( 
		NULL,                   // default security attributes
		0,                      // use default stack size  
		(LPTHREAD_START_ROUTINE) (cageArray[curThreadSpot]->doWork),       // thread function name
		cageArray[curThreadSpot],          // argument to thread function 
		0,                      // use default creation flags 
		&threadID);   // returns the thread identifier 

	
}


int server::getNextEmptyThreadSpot()
{
	
	int ret = currentCage;
	currentCage++;
	return ret;

}




