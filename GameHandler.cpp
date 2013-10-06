#include "StdAfx.h"
#include ".\gamehandler.h"


#define MAXGAMES 255

#include <iostream>

using namespace std;

//<TO DO> To get to the next game in the array, 
//I increment the current pointer to that array and use that
//game.  This is a problem because the server won't be able
//to serve more than 255 games at one time.
GameHandler::GameHandler(void)
{
	//INIT NICKS_GRAPHICS STUFF HERE

	//init the critical sections
	InitializeCriticalSection(&m_CriticalSection); 

	//arrays of each game class that people are currently playing
	//current game class that people are loading up
	Game_solo = new Game* [MAXGAMES];
	Game_1v1 = new Game* [MAXGAMES];
	Game_2v2 = new Game* [MAXGAMES];
	Game_4FFA = new Game* [MAXGAMES];

	//memset all game arrays to zero
	memset (Game_solo,0,(MAXGAMES * sizeof(Game*)));
	memset (Game_1v1,0,(MAXGAMES * sizeof(Game*)));
	memset (Game_2v2,0,(MAXGAMES * sizeof(Game*)));
	memset (Game_4FFA,0,(MAXGAMES * sizeof(Game*)));

	//cur games that need to be filled with players still
	Cur_solo = 0;
	Cur_1v1 = 0;
	Cur_2v2 = 0;
	Cur_4FFA = 0;

	debug = 0;
}


	//returns 0 for sucess or 1 for failure
	int GameHandler::disconnect(Game* game, SOCKET* clientSocket)
	{
		//lock
		EnterCriticalSection(&m_CriticalSection); 

		//decrement the number of players currently playing
		game->numPlayers--;

		cerr<<"ENTER DISCONNECT PLAYERS:::"<<game->numPlayers<<endl;

		//remove their socket from the game
		for(int i = 0; i< game->playersNeeded; i++)
		{
			if(game->Clients[i] == clientSocket)
			{
				game->Clients[i] = 0;
				cerr<<"player::"<<i<<" REMOVED"<<endl;
				//remove my ship from the game
				
				game->playerMan->disconnectPlayer( &(game->ObjMan->privStartPlayerShip[i]) );

				break;
			}
		}

		//if there are no more players in the game, delete it from the array
		if(game->numPlayers <1)
		{
			//find the game and set the pointer in the array it came from to zero
			int found = 0;
			if(!found)
				for(int i = 0; i<= Cur_solo; i++)
				{
					if(game == Game_solo[Cur_solo])
					{
						found = 1;
						Game_solo[Cur_solo] = 0;
					}

				}
			if(!found)
				for(int i = 0; i<= Cur_1v1; i++)
				{
					if(game == Game_1v1[Cur_1v1])
					{
						found = 1;
						Game_1v1[Cur_1v1] = 0;
					}
					
				}
			if(!found)
				for(int i = 0; i<= Cur_2v2; i++)
				{
					if(game == Game_2v2[Cur_2v2])
					{
						found = 1;
						Game_2v2[Cur_2v2] = 0;
					}

				}
			if(!found)
				for(int i = 0; i<= Cur_4FFA; i++)
				{
					if(game == Game_4FFA[Cur_4FFA])
					{
						found = 1;
						Game_4FFA[Cur_4FFA] = 0;
					}

				}

			cerr<<"DELETEING GAME"<<endl;
			delete game;
		}

		//unlock
		LeaveCriticalSection(&m_CriticalSection); 

		return 0;
	}

GameHandler::~GameHandler(void)
{
	DeleteCriticalSection(&m_CriticalSection); 
	delete [] Game_solo;
	delete [] Game_1v1;
	delete [] Game_2v2;
	delete [] Game_4FFA;
}
//makes a new game of the type they want or 
//returns the game they joined or 0 if err
Game* GameHandler::joinGame(int gameType, SOCKET* curClient, int* pPlayerID)
{


	//lock
	EnterCriticalSection(&m_CriticalSection); 
	cerr<<"joinGame :: "<<gameType<<endl;

	int err = 0;

	//join command for 1 player alone... used for testing
	if(gameType == 0)
	{	
		//create new game if there is no game to join
		if(Game_solo[Cur_solo] == 0)
		{
			if(MAXGAMES <= Cur_solo)
			{
				if(debug)
					cerr<<"too many solo games playing, max number of games is: "<<MAXGAMES<<endl;

				//unlock
				LeaveCriticalSection(&m_CriticalSection); 

				return 0;
			}
			Game_solo[Cur_solo] = new Game(1, NICKS_GRAPIHCS);
		}
		
		Game* ret = Game_solo[Cur_solo];

		//increment the location where we are because we have a full game...
		//move on
		if(Game_solo[Cur_solo]->joinGame(curClient, pPlayerID) == 1)
		{
			cerr<<"CREATING GAME"<<endl;
			Cur_solo++;
		}
		//unlock
		LeaveCriticalSection(&m_CriticalSection); 
		//link cage and the game that they want to join
		return ret;

	}
	//join command for 1v1
	else if(gameType == 1)
	{

		cerr<<"Game Type 1"<<endl;

		//create new game if there is no game to join
		if(Game_1v1[Cur_1v1] == 0)
		{
			if(MAXGAMES <= Cur_1v1)
			{
				if(debug)
					cerr<<"too many 1v1 games playing, max number of games is: "<<MAXGAMES<<endl;
				//unlock
				LeaveCriticalSection(&m_CriticalSection); 
				return 0;
			}
			Game_1v1[Cur_1v1] = new Game(2, NICKS_GRAPIHCS);
		}
		Game* ret = Game_1v1[Cur_1v1];
		cerr<<"GAME POINTER::"<<ret<<endl;
		//increment the location where we are because we have a full game...
		//move on
		if(Game_1v1[Cur_1v1]->joinGame(curClient, pPlayerID) == 1)
		{
			Cur_1v1++;

		}
		//unlock
	
		LeaveCriticalSection(&m_CriticalSection); 
		//link cage with game
		return ret;

	}
	//join command for 2v2
	else if(gameType == 2)
	{

		//create new game if there is no game to join
		if(Game_2v2[Cur_2v2] == 0)
		{
			if(MAXGAMES <= Cur_2v2)
			{
				if(debug)
				cerr<<"too many 2v2 games playing, max number of games is: "<<MAXGAMES<<endl;
				//unlock
				LeaveCriticalSection(&m_CriticalSection); 
				return 0;
			}
			Game_2v2[Cur_2v2] = new Game(4, NICKS_GRAPIHCS);
		}

		Game* ret = Game_2v2[Cur_2v2];

		//increment the location where we are because we have a full game...
		//move on
		if(Game_2v2[Cur_2v2]->joinGame(curClient, pPlayerID) == 1)
		{
			Cur_2v2++;

		}
		//unlock
		LeaveCriticalSection(&m_CriticalSection); 
		return ret;

	
	}
	//join command for 4 way free for all
	else if(gameType == 3)
	{


		int foundGame = 0;
		Game* ret = 0;
		
		//scroll though all the games to find a spot in a game already made
		for(int i = 0; i< Cur_4FFA; i++)
		{

			if(Game_4FFA[i]->numPlayers < Game_4FFA[i]->playersNeeded)
			{
				Game_4FFA[i]->joinGame(curClient, pPlayerID);
				ret = Game_4FFA[i];
				foundGame = 1;
			}

		}


		if(foundGame == 0)
		{

			//create new game if there is no game to join
			if(Game_4FFA[Cur_4FFA] == 0)
			{
				if(MAXGAMES <= Cur_4FFA && debug)
				{
					if(debug)
						cerr<<"too many 4FFA games playing, max number of games is: "<<MAXGAMES<<endl;
					//unlock
					LeaveCriticalSection(&m_CriticalSection); 
					return 0;
				}
				Game_4FFA[Cur_4FFA] = new Game(4, NICKS_GRAPIHCS);
			}

			ret = Game_4FFA[Cur_4FFA];

			//increment the location where we are because we have a full game...
			//move on
			if(Game_4FFA[Cur_4FFA]->joinGame(curClient, pPlayerID) == 1)
			{
				Cur_4FFA++;

			}
		}
		//unlock
		LeaveCriticalSection(&m_CriticalSection); 
		return ret;


	}
	//unlock
	LeaveCriticalSection(&m_CriticalSection); 
	return 0;
}