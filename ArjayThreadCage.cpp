#include "StdAfx.h"
#include ".\arjaythreadcage.h"


//for timer
#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <tchar.h>


ArjayThreadCage::~ArjayThreadCage(void)
{
	//free lock
	DeleteCriticalSection(&m_CriticalSection); 
}

	/**************************************************************************
	*					ArjayThreadCage
	*
	*	Threads will go to this class to do all their work until the game is
	*	over
	*
	*	we need this wrapper class because all vars inside a
	*	static class are also on the data section.  need this or we get 
	*	cross talk
	*
	**************************************************************************/






	ArjayThreadCage::ArjayThreadCage(ThreadInfo* TI)
	{

		//init lock
		InitializeCriticalSection(&m_CriticalSection); 


		//get the info from the struct
		ClientSocket = TI->ClientSocket;

		GH = TI->GH;

		//client is prejoined
		clientState = 0;

		//set joinedGame to null when there is no game that the client joined
		joinedGame = 0;

		//set the debug messages on
		debug = 0;


		countTillNextKeepAlive = 0;

	}
	





	//creates the keep alive thread
	//sends 0 if not joined yet and 1 if
	void ArjayThreadCage::createKeepAlive()
	{


		//save thread in case we impliment event driven threads.	
		loopThread[0] = CreateThread( 
			NULL,       // default security attributes
			0,          // default stack size
			(LPTHREAD_START_ROUTINE) keepAlive, 
			this,       // no thread function arguments
			0,          // default creation flags
			&loopThreadID); // receive thread identifier

    }





	void ArjayThreadCage::waitTenMiliSeconds()
	{
		//wait for 10 mili-seconds
//		Sleep(10); 

	}

	/**************************************************************************
	*					doCommand
	*
	*	Description: Does a specified command
	*
	*	data:the data recived from the client
	*
	*	dataSize: address to the size of the data that is going to be sent to the 
	*			client
	*
	*	clientSocket: the clientSocket that is doing the "doCommand"
	*	
	*	returns: returns the address to the information that is needed to 
	*			be sent to the client.
	*
	**************************************************************************/
void* ArjayThreadCage::doCommand( void* recivedData,  int* sendSize, ArjayThreadCage* curCage )
{
	
	//take first byte of test as the command
	char command = *((char*)recivedData);
	
	//get the data that the client sent us
	//convert data to char*, add 1 to skip command
	char* data =   (((char*) recivedData)+1);

	if(debug)
		cerr<<"ArjayThreadCage::doCommand::Command("<<char(command+'0')<<") :: DATA::"<<data<<endl;

	//join Game Command
	if(command == 0)
	{
		//only join a game if the client is not currently in the play state

		if(curCage->clientState != 2)
		{
			//lock
			EnterCriticalSection(&m_CriticalSection); 


			//get which kind of joined game type we want to join
			int joinType= * ((int*) data);
			
			//the game type has to be between 0 and 3 because we only support 4 game types so far
			if( 0 <= joinType && 3>=joinType )
			{
				if(debug)
				{
					cerr<<"Joining Game Type: "<<joinType<<endl;
				}



				int test = 0;


				//call game handler's joinGame func with the joinType.
				//get joined game
				curCage->joinedGame = curCage->GH->joinGame(joinType, curCage->ClientSocket, (&playerID));


				if(curCage->joinedGame == NULL)
					cerr<<"game pointer is NULL BADDDD =(\n";

				//the command that we want to send to the server
				char command = 4;

				char* dataToSendToServer = new char [5];
		
				memcpy (dataToSendToServer, (&command) ,1);
				memcpy ( (dataToSendToServer+1) , (&playerID) ,sizeof(int) );
				

				//set size of sendSize for the sizing to the client
				(*sendSize) = sizeof(int) + sizeof(char);
				cerr<<"PLAYER ID::"<<playerID<<endl;

				//update client state
				curCage->clientState = 1;

				//unlock
				LeaveCriticalSection(&m_CriticalSection); 
				

				return dataToSendToServer;
			}

			//unlock
			LeaveCriticalSection(&m_CriticalSection); 
		}
		else
		{
			cerr<<"ERR:::CLIENT IS TRYING TO JOIN A GAME WHILE IN A GAME"<<endl;
			//if your reading this, you want to let a player join a game while in a game.
			//If this is modified, you'll have the update thread possibly reading from a null
			//pointer and the thread that deals with async commands also modifying it
			//protect the joinGame var
		}

	}
	else if(11 == command)
	{

		if(2 == curCage->clientState)
		{


			//lock
			//EnterCriticalSection( &(this->joinedGame->m_CriticalSection));


			D3DXVECTOR2 movementControls;
			memcpy ( (&movementControls), data, sizeof(D3DXVECTOR2) );

			//move to next controls
			data += sizeof(D3DXVECTOR2);
			D3DXVECTOR2 fireControls;
			memcpy ( (&fireControls), data, sizeof(D3DXVECTOR2) );


			PlayerShip* playerShip = joinedGame->ObjMan->privStartPlayerShip;




			playerShip[playerID].movement.x = movementControls.x;
			playerShip[playerID].movement.y = movementControls.y;

			playerShip[playerID].fireDirection.x = fireControls.x;
			playerShip[playerID].fireDirection.y = fireControls.y;

			//unlock
			//LeaveCriticalSection( &(this->joinedGame->m_CriticalSection) );

		}
		
	}
	//TESTING::::, print off msg, increment, send again
	else if(command == 200)
	{
		//echo off the data
		cerr<<"COMMAND 200::"<<data<<endl;
	}
	else
	{
		char com = command + '0';
		cerr<<endl;
		cerr<<"COMMAND:::"<<com;
		cerr<<endl;
		cerr<<"DATA:::"<<recivedData;
		cerr<<endl;

		if(joinedGame == 0)
		{
			cerr<<"Trying to do stuff on a game that doesn't exist"<<endl;
		}
		cerr<<"ArjayThreadCage::doCommand::Command("<<char(command+'0')<<") :: DATA::"<<data<<endl;
		cerr<<"::::ERROR COMMAND RECIVED THAT IS UNKNOWN::::"<<endl;
	}

	//if it makes it here, send nothing to client
	(*sendSize) = 0;
	return (void*) 0;

}


