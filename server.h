/****************************************************************************

							Team ACK DNY
							CSE 125, Spring 2008
							April 6, 2008
							
				

File Name:	server.h
Description:	This program is the server.  common usage is declare it
				then open up the port you want to host on with 
				openAndAcceptNewClients, and pass in the function you want
				to run whenever the server/host gets information from it's
				clients.  Then closing the connection

				A special feature is sending information.  This may be 
				used in our lobby.

****************************************************************************/




#pragma once
#ifndef _SERVER_
#define _SERVER_





//#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

//the number of players we can accomadate before the server crashes
//because we don't delete the cages.
#define MAXCAGE 255




#include "ArjayThreadCage.h"
#include "ThreadInfo.h"
#include "GameHandler.h"
#include "Game.h"

#include "connection.h"


//max threads we can cycle through






class server
{
	

	GameHandler* GH;

	//array of commands
	//commands: updateInformation, joinGame




	int debug;
	WSADATA wsaData;
    SOCKET* ListenSocket;

	struct addrinfo *result,
		hints;

    int iResult, iSendResult;

	int currentCage;



public:



	//all the threads we have so we can wait for all the threads to end before we close the 
	//threads before we close the server..... not implimented during first 10 weeks.
	HANDLE threadArray[MAXCAGE];

	//all threads that have gone through this server
	ArjayThreadCage** cageArray;


	//This var tells us if the server is running or not running.
	//set this to false if you want to turn off the server
	int isOn;

	//constructor, nothing special done here
	server(void);

	//destructor, where we're suppose to wait for the client threads to end
	//but we don't in this version.
	~server(void);


	/**************************************************************************
	*					spawnPlayerThread
	*
	*	Description: Spawns a new thread and binds it to an ArjayThreadCage.
	*				ArjayThreadCage takes care of that client.
	*				
	*
	*	clientSocket: The socket that we use to communicate to the client
	*
	**************************************************************************/
	void spawnPlayerThread(SOCKET* clientSocket);

	/**************************************************************************
	*					openAndAcceptNewClients
	*
	*	Description: Opens a connection and starts accepting clients
	*				Once a client is accepted, the server will continually
	*				communicate to the client until the client until the 
	*				client closes the connection
	*	
	*	port: the port you want your server to run on	
	*   func: the function you want to run every time a client sends
	*		information.  Every time a client sends info, func is run on 
	*       the information that the client gives us.  It takes in a void*
	*		which is the information that the client gives the server and
	*		it returns a void* which is what the host or server gives to
	*		the client.
	*
	*		
	*		openAndAcceptsNewClients will delete the data. 
	*
	**************************************************************************/
	int openAndAcceptNewClients(char* port);

	//gets the next spot in avalible threads
	//We never get rid of the threads of this array... The only time we use this
	//is when the server is shut down we should wait for all the threads to complete
	//this wont happen in the limited time we have so when the server ends,
	//all other threads will crash until it's fixed.
	int getNextEmptyThreadSpot();

};
#endif /* _SERVER_ */