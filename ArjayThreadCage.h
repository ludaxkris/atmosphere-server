#pragma once

#include "ThreadInfo.h"
#include "GameHandler.h"
#include "Game.h"

#include "connection.h"

#include <iostream>
using namespace std;



#define numToWaitKeepAlive 10

using namespace System::Threading;

class ArjayThreadCage
{

	
public:
	CRITICAL_SECTION m_CriticalSection;

	GameHandler* GH;
	
	int playerID;


	SOCKET* ClientSocket;

	Game* joinedGame;
	int debug;

	connection con;


	//0 is prejoined
	//1 is joined but not enough clients
	//2 is playing game
	//3 is game ended
	int clientState;


	//sync communication thread, aka loop thread
	HANDLE loopThread [1];
	DWORD loopThreadID;


	int countTillNextKeepAlive;

	
	ArjayThreadCage(ThreadInfo* TI);

	//returns 0 if the game ended okay
	//returns 1 if there was some other kind of error

	//Secuirty modify: for secuiryt we should know the size of the data comming in
	//as well as the size going out.
    static DWORD doWork(LPVOID lpParam) 
    {
		
		ArjayThreadCage *ATCInstance=(ArjayThreadCage *)lpParam;

		if(ATCInstance->debug)
			cerr<<"ArjayThreadCage::doWork(LPVOID lpParam)"<<endl;

		//create new thread
		ATCInstance->createKeepAlive();


		//call doCommands as long as client is connected
		char* recivedData;
		int end =0;

		do {
			void* dataToSend;
			int sendSize = 0;

	
			//get data from client or wait till there is data from client
			recivedData = (char*)ATCInstance->con.reciveDataFromClient((ATCInstance->ClientSocket), (&sendSize), (&end));
			if(ATCInstance->debug)
				cerr<<"RECIVED DATA::"<<endl;
			if( (int)recivedData > 0)
			{
				//if the recived data has a command to parse
				if(sendSize)
				{
					dataToSend = ATCInstance->doCommand( ((void*)recivedData),  &sendSize, ATCInstance );
	
					if(sendSize >0)
						ATCInstance->con.sendDataToClient(ATCInstance->ClientSocket, dataToSend, sendSize);

					
				}
			}
			else 
			{
				//if(ATCInstance->debug)
					cerr<<"END by fin sent?"<<endl;
				end = 1;
			}

			//if there's somthing to delete, delete it
			if(recivedData > 0)
				delete recivedData;

			//keep going while end is not 1 (true)
		} while ( end != 1);

//HACK, wait for multiple objects command not working
//for our 15 min demo, this is good enough.
Sleep(1000);



		//wait for loop thread to end before ending the current thread
		WaitForMultipleObjects( 
        1,           // number of objects in array
        ATCInstance->loopThread,     // array of objects
        TRUE,       
        INFINITE);       // wait indefinatly until threads end


		//game is over and threads won't be updating any memory, client disconnected
	
		//if we have joined a game, then disconnect from it, else do nothing
		if(ATCInstance->joinedGame)
			ATCInstance->GH->disconnect(ATCInstance->joinedGame, ATCInstance->ClientSocket);
		
		//return car key
		ATCInstance->joinedGame = 0;

		//disconnect from socket
		ATCInstance->con.closeConnection(ATCInstance->ClientSocket);
		

		return 0;

    }
	
	void createKeepAlive();


    //loops every 10 miliseconds to check if the game started
	//sends keep Alive message to client every 1 second
	//if game started, call playGame()
    static DWORD ArjayThreadCage::keepAlive(LPVOID lpParam)
    {


		ArjayThreadCage *ATCInstance=(ArjayThreadCage *)lpParam;

		if(ATCInstance->debug)
			cerr<<"ArjayThreadCage::keepAlive(LPVOID lpParam)"<<endl;

		//0 is prejoined
		//1 is joined but not enough clients
		//2 is playing game
		//3 is game ended
		while(ATCInstance->clientState <3)
		{
			//Lock 4 join change
			EnterCriticalSection(&(ATCInstance->m_CriticalSection)); 

			if(ATCInstance->clientState == 0)
			{
				if(ATCInstance->countTillNextKeepAlive >	numToWaitKeepAlive)
				{
					if(ATCInstance->debug)
						cerr<<"Sending Keep Alive Before Join"<<endl;

					//send keepAlive command (prejoin)
					//command 0 is keep alive before join
					char command = 0;
					
					//if send failed then return/end thread.  client closed connection
					if(ATCInstance->con.sendDataToClient(ATCInstance->ClientSocket, (void*)0, 0, command))
					{
						return 0;
					}


				}
				ATCInstance->countTillNextKeepAlive++;
			}
			else if(ATCInstance->clientState ==1)
			{

				if(ATCInstance->countTillNextKeepAlive >	numToWaitKeepAlive)
				{
					ATCInstance->countTillNextKeepAlive = 0;
					if(ATCInstance->debug)
						cerr<<"Sending Keep Alive after Join"<<endl;

					//send keepAlive command (not enough clients)
					//command 1 is keep alive after join (not enough clients)
					char command = 1;
					//if send failed then return/end thread.  client closed connection
					if(ATCInstance->con.sendDataToClient(ATCInstance->ClientSocket, (void*)0, 0, command))
					{
						//problem/error/ when cycling through multiple 1v1 games, disconnects player 0, trying to not return when keep alive fails
						cerr<<"SEND FAILED, killing keep alive message loop"<<endl;
						
						return 0;
					}
				}
				ATCInstance->countTillNextKeepAlive++;

				//The game has started!, set client state to 2
				if(ATCInstance->joinedGame->state == 1)
					ATCInstance->clientState = 2;
			}
			else if(ATCInstance->clientState ==2)
			{

				//move to playGame, game will handle it, end thread
				LeaveCriticalSection(&(ATCInstance->m_CriticalSection)); 
				return 0;

			}

			//unlock 4 join 
			LeaveCriticalSection(&(ATCInstance->m_CriticalSection)); 
			//ATCInstance->waitTenMiliSeconds();
			Sleep(20);
		}


		return 0;
	
    }



	void waitTenMiliSeconds();
	
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
	void* doCommand( void* recivedData,  int* sendSize, ArjayThreadCage* curCage );

	~ArjayThreadCage(void);


};
