#pragma once

#include "connection.h"
#include <iostream>

#include "./Objects/ObjectManager.h"
#include "./GameLogic/ProjectileManager.h"
#include "./GameLogic/ProjectileManager.h"
#include "./GameLogic/ItemManager.h"
#include "./GameLogic/BaseManager.h"
#include "./GameLogic/PlayerManager.h"
#include "./Collision.h"

#define UPDATETIME 15

using namespace std;
	//game class (take in # of clients needed to start game)

//Warnings::::
//no support for thred array re-use... only 255 people can play atm
	//fix getNextEmptyThreadSpot
class Game
{	
	int currentThread;
	int getNextEmptyThreadSpot();

public: 
	//for locks
	CRITICAL_SECTION m_CriticalSection;

	PlayerManager * playerMan;
	BaseManager * baseMan;
	ItemManager * itemMan;
	ProjectileManager* projMan;
	ObjectManager* ObjMan;

	//sync communication thread, aka loop thread
	HANDLE loopThread [1];
	DWORD loopThreadID;

	connection con;
	//how many bytes we are shareing across from the server to the client.
	//REMEMBER pointers are 4 bytes on 32 bit machines and 8 bits on 64 bit machines.
	int sharedMemorySize;
	SOCKET ** Clients;

	//for all the clients to request a removeal of certin data when explosions are complete
	


	int debug;


	~Game(void);


	
	//shared memory -- shared memory pushed to clients
	void* sharedMemory;

	//# users in game -- the number of users that is in the game
	int numPlayers;

	//# players needed to start the game
	int playersNeeded;

	//Game modes
	int gameTimeStart;
	int gameTimeDuration;
	int gameWinScore;

	//started -- if the game started yet
	// 0 = not started
	//1 = started
	int state;



	Game(int numPlayersNeeded, void* NICKS_GRAPHICS);


	//joinGame() -- adds to socket[], increments # shared users, calls start game when # is meet
	//since we're in the call between the mutexes, this is guarenteed to have only one 
	//thread in it at once.

	int joinGame(SOCKET* curClient,int* pPlayerID);


	//ends the game.
	void endGame();

	//put int flag var inside player to notify state (hit, explode, ect)
	//phsycs, pass it an array to collide structs to tell what has colided
	//collide struct = two spaceobj pointers
	Collision* doPhysics(Collision* collision);

	//do game logic
	//inside do game logic , it takes in an array of collide structs
	//inside game logic if a bullet colides with a ship, it's hp goes down
	//and the bullet disapears
	void doCollision(Collision* collision);


    static DWORD playGame(LPVOID lpParam)
    {

		Game *GInstance=(Game *)lpParam;

		//do while game is still playing
		//update mem while game is still on
		int oldTime;

		while(GInstance->state == 1)
		{
			oldTime = timeGetTime();
		
			//lock
			EnterCriticalSection(&(GInstance->m_CriticalSection));
			
			GInstance->ObjMan->mary->currentTime = timeGetTime();

			//update memory
			updateMemory(GInstance);			

			//////////////////////////
			//do game logic			
			GInstance->GameLogic();

			//unlock
			LeaveCriticalSection(&(GInstance->m_CriticalSection)); 
//			GInstance->waitTenMiliSeconds();
			int timeDiff = (timeGetTime() - oldTime);
			if((UPDATETIME - timeDiff) > 0)
				Sleep(UPDATETIME - timeDiff);
		
		}
		//game auto ends when there are no more clients to server
		return 0;
    }

	void GameLogic();
	//void doController();


	static void updateMemory(Game *GInstance)
	{
		
		if(GInstance == 0)
			return;

		if(GInstance->debug)
			cerr<<"Sending Memory Update"<<endl;


		//command that details to client this message is updating shared memory.
		char command = 3;

				//send update to all clients
				for(int i = 0; i<GInstance->playersNeeded; i++)
				{

					MaryMaid* mary =  (MaryMaid*)(GInstance->ObjMan->m_PublicMem);


					//if send failed then return/end thread.  client closed connection
					if(GInstance->Clients[i] != 0 && GInstance->con.sendDataToClient(GInstance->Clients[i], GInstance->sharedMemory, GInstance->sharedMemorySize, command))
					{
						cerr<<"CLIENT DISCONNECT!!!!  CLIENT ID:::"<<i<<endl;
						GInstance->con.closeConnection(GInstance->Clients[i]);
						//GInstance->Clients[i] = 0;
					}
					else
					{
						//cerr<<"UPDATING CLIENT:::"<<i<<endl;
					}
					
				}
			
				//clear flags off player ship
				for (int i=0; i < GInstance->ObjMan->numPlayerShip; i++)
				{
					PlayerShip * curPlayer = &GInstance->ObjMan->privStartPlayerShip[i];
					((SharedInfoPlayerShip*)curPlayer->sharedInfo)->playerFlag = 0;
				}

				//clear flags off projectiles
				for(int i = 0 ; i <GInstance->ObjMan->numProjectile; i++)
				{
					Projectile* curProj = &(GInstance->ObjMan->privStartProjectile[i]);
					
					//if((int)((SharedInfoProjectile*)curProj->sharedInfo)->projFlag != 0)
					//	cerr<<(int)((SharedInfoProjectile*)curProj->sharedInfo)->projFlag<<endl;
	
					((SharedInfoProjectile*)curProj->sharedInfo)->projFlag = 0; 
				}

				//clear flags off base
				//clear flags off projectiles
				for(int i = 0 ; i <GInstance->ObjMan->numBase; i++)
				{
					Base* curBase = &(GInstance->ObjMan->privStartBase[i]);
					
					//if((int)((SharedInfoProjectile*)curProj->sharedInfo)->projFlag != 0)
					//	cerr<<(int)((SharedInfoProjectile*)curProj->sharedInfo)->projFlag<<endl;
	
					((SharedInfoBase*)curBase->sharedInfo)->baseFlag = 0; 
				}
	}


	void waitTenMiliSeconds();
	Collision* deleteCollisionInvolvingObj(Collision* collision, SpaceObject* obj);
};