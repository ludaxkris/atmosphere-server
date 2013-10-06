#include "StdAfx.h"
#include ".\game.h"
#include "connection.h"
#include <math.h>
#using <mscorlib.dll>

#define SHAREDMEMSIZE 10000



	Game::~Game(void)
	{
		//free lock
		DeleteCriticalSection(&m_CriticalSection); 
		delete Clients;
		Clients = 0;
	}

	Game::Game(int numPlayersNeeded, void* NICKS_GRAPHICS)
	{
		ObjMan = new ObjectManager();
		projMan = new ProjectileManager(ObjMan);
		baseMan = new BaseManager(ObjMan);
		itemMan = new ItemManager(ObjMan);
		playerMan = new PlayerManager(ObjMan);
		
		//turn debug messages on
		debug = 0;
		//init lock
		InitializeCriticalSection(&m_CriticalSection); 
		sharedMemorySize = ObjMan->totalSharedMemSize;
		playersNeeded = numPlayersNeeded;
		numPlayers = 0;
		state = 0;
		sharedMemory = ObjMan->m_PublicMem;
		Clients = (SOCKET**) (new SOCKET* [numPlayersNeeded]);
		


		for(int i = 0 ; i<numPlayersNeeded; i++)
			Clients[i] = 0;

	}
	

	//joinGame() -- adds to socket[], increments # shared users, calls start game when # is meet
	//since we're in the call between the mutexes, this is guarenteed to have only one 
	//thread in it at once.

	//returns 1 if we have enough players to play the game
	//returns 0 if we don't have enough players (aka full game)
	int Game::joinGame(SOCKET* curClient, int* pPlayerID)
	{
		if(debug)
			cerr<<"Game::joinGame(SOCKET* curClient)"<<endl;

		//lock
		EnterCriticalSection(&m_CriticalSection); 
	

		//get the socket that is empty and this player's id is now that number
		//iterate through all the sockets
		for(int i = 0; i< playersNeeded; i++)
		{
			if(Clients[i] == 0)
			{
				cerr<<"MADE IT HERE"<<endl;
				Clients[i] = curClient;
				(*pPlayerID) = i;
				break;
			}
		}
				
		//(*pPlayerID) = this->numPlayers;


		//init a new player for the player comming in
		PlayerShip* curPlayer = ObjMan->reCreatePlayerShip(*pPlayerID);

		//Clients[numPlayers] = curClient;
		numPlayers++;
		cerr<<"NUM PLAYERS::"<<numPlayers<<endl;
		
		//if game not started and we have enough players

//		if(state != 1 && numPlayers >= playersNeeded)
// <TO DO> replace this if statment with the one above it before we do DEMO!!!!!!!
//ARJAY TO DO
//DO THIS
//DON'T FORGET
		if(state != 1 )
		{
			//Set game start time
			gameTimeStart = ObjMan->mary->currentTime;

			state = 1;

			//The Game has started, spawn playGame Thread
			loopThread[0] = CreateThread( 
				NULL,       // default security attributes
				0,          // default stack size
				(LPTHREAD_START_ROUTINE) playGame, 
				this,       // no thread function arguments
				0,          // default creation flags
				&loopThreadID); // receive thread identifier
			//unlock
			LeaveCriticalSection(&m_CriticalSection); 
			return 1;
		}
		//unlock
		LeaveCriticalSection(&m_CriticalSection); 
		return 0;
	
	}

	//ends the game
	void Game::endGame()
	{
		if(debug)
			cerr<<"Game::endGame()"<<endl;
		cerr<<"GAME STATE SET TO 2END GAME"<<endl;
		//lock
		EnterCriticalSection(&m_CriticalSection); 
		
		//state = 2;
		
		//unlock
		LeaveCriticalSection(&m_CriticalSection); 
	}

	void Game::waitTenMiliSeconds()
	{
		if(debug)
			cerr<<"Game::waitTenMiliSeconds()"<<endl;

		//wait for 10 mili-seconds
		//Sleep(1); 

	}

	void Game::GameLogic() {
		//Set game mode.
		/////////////////////////////////

		//put inside config, do in game create
		int mode = 1;
		gameWinScore = 5;
		gameTimeDuration = 10000;

		//check for end condition
		/*switch(mode) {
			case 0: {//Timer based game
				int gameTime = ObjMan->mary->currentTime - gameTimeStart;
				if(gameTime > gameTimeDuration) {
					//End game
					cerr<<"Game::Game Over"<<endl;
					endGame();
				}
				break;
					}

			case 1:	{//Point based game
				//playerMan check for winner
				int winner = playerMan->checkPlayerScore(gameWinScore);
				if( winner != -1 ) {
					cerr<<"Game::Game over"<<endl<<"Player "<<winner<<" wins!"<<endl;
					endGame();
				}
				break;
					}

			default:
				cerr<<"Game::Game Mode incorrect!"<<endl;
				break;
		}
		*/

		//Updates
		////////////////////////////////////////////////
		//Player updates - move ship
		playerMan->update();
		
		//Base updates - give out points
		baseMan->update();

		//Item respawn if possible
		itemMan->update();

		//Projectile updates - kill projectile if necessary
		//move projectile
		projMan->update();

		//Get collision list		
		Collision* collision = 0;
		collision = doPhysics(collision);

		//Do collision
		doCollision(collision);
	}

	
	//put int flag var inside player to notify state (hit, explode, ect)
	//phsycs, pass it an array to collide structs to tell what has colided
	//collide struct = two spaceobj pointers
	Collision* Game::doPhysics(Collision* collision)
	{
		//iterate through live bullets
		for(int curIndexProj = ObjMan->mary->projectileStart; -1 != curIndexProj; curIndexProj = ObjMan->privStartProjectile[curIndexProj].sharedInfo->next)
		{
			Projectile* curProj = &(ObjMan->privStartProjectile[curIndexProj]);
			D3DXVECTOR3 projPos = curProj->getPosition();

			//iterate through all the ships
			for(int curIndexShip = ObjMan->mary->playerShipStart ; -1 != curIndexShip ; curIndexShip = ObjMan->privStartPlayerShip[curIndexShip].sharedInfo->next)
			{
				PlayerShip* curPlayerShip = &(ObjMan->privStartPlayerShip[curIndexShip]);
				D3DXVECTOR3 playerPos = curPlayerShip->getPosition();

				float x = projPos.x - playerPos.x;
				float y = projPos.y - playerPos.y;
				float z = projPos.z - playerPos.z;
				float distance = sqrt(x*x + y*y + z*z);

				if((curPlayerShip->radius+curProj->radius) >= distance && ((SharedInfoProjectile*)curProj->sharedInfo)->shipID != curPlayerShip->index)
				{
					if(NULL == collision)
					{
						collision = new Collision(curPlayerShip, curProj);
					}
					else
					{
						//insert at start
						Collision* newCol = new Collision(curPlayerShip, curProj);
						newCol->next = collision;
						collision = newCol;
					}
				}
			}
		}

		//iterate through all the bases
		for(int curIndexBase = 0;
			curIndexBase < ObjMan->numBase; 
			curIndexBase++)
		{
			Base* curBase = &(ObjMan->privStartBase[curIndexBase]);
			D3DXVECTOR3 basePos = curBase->getPosition();

			//iterate through all the ships
			for(int curIndexShip = ObjMan->mary->playerShipStart ; 
				-1 != curIndexShip ; 
				curIndexShip = ObjMan->privStartPlayerShip[curIndexShip].sharedInfo->next)
			{
				PlayerShip* curPlayerShip = &(ObjMan->privStartPlayerShip[curIndexShip]);
				D3DXVECTOR3 playerPos = curPlayerShip->getPosition();

				float x = basePos.x - playerPos.x;
				float y = basePos.y - playerPos.y;
				float z = basePos.z - playerPos.z;
				float distance = sqrt(x*x + y*y + z*z);

				//if it collides
				if((curPlayerShip->radius+curBase->radius) >= distance)
				{
					if(NULL == collision)
					{
						collision = new Collision(curPlayerShip, curBase);
					}
					else
					{
						//insert at start
						Collision* newCol = new Collision(curPlayerShip, curBase);
						newCol->next = collision;
						collision = newCol;
					}
				}
			}
		}

		
		return collision;
	}

	//do game logic
	//inside do game logic , it takes in an array of collide structs
	//inside game logic if a bullet colides with a ship, it's hp goes down
	//and the bullet disapears
	void Game::doCollision(Collision* collision)
	{
		//go through all collisions
		Collision* oldCol = collision;
		while(collision)
		{
			SpaceObject* obj1 = collision ->obj1;
			SpaceObject* obj2 = collision ->obj2;

			//first is always projectile
			if(collision->obj2->isProjectile())
			{
				obj1= collision->obj2;
				obj2= collision->obj1;
			}

			//first is always base			
			if(collision->obj2->isBase())
			{
				obj1= collision->obj2;
				obj2= collision->obj1;
			}
			
			//if a projectile hits a playership then make the projectile disapear
			if(obj1->isProjectile() && obj2->isPlayerShip())
			{
				//obj1 = projectile
				//obj2 = ship
				projMan->doCollision ((Projectile *)obj1, (Ship *)obj2);

				//if projectile is not a fire remove from collision list
				//if(((Projectile *)obj1)->)
				collision = deleteCollisionInvolvingObj(collision, obj1);
		
				//else
				//delete the collsion
				/*oldCol = collision;
				collision = collision->next;

				delete oldCol;
				*/

			}
			//if it's a base and playership collsion	
			else if(obj1->isBase() && obj2->isPlayerShip())
			{
				this->baseMan->doCollision((Base *)obj1, (PlayerShip *)obj2);

				//delete the collsion
				oldCol = collision;
				collision = collision->next;
				delete oldCol;
			}
			else
			{
				cerr<<"Found undefined collision in doCollision.\n"<<endl;
			}
		}
	}
	
	//returns the head of the collisions or return NULL for empty list
	Collision* Game::deleteCollisionInvolvingObj(Collision* collision, SpaceObject* obj)
	{
		Collision* first = 0;
		//go through all collisions
		while(collision)
		{	
			//if the object is eaither object 1 or object 2, delete the collision
			if(collision->obj1 == obj || collision->obj2 == obj)
			{
				Collision * oldCol = collision;
				collision = collision->next;
				delete oldCol;
			}
			//it's the first collision that we are not deleting, so we need to save it to return it
			else if(first ==0)
			{
				first = collision;
				collision = collision->next;
			}
			//we're not deleting the collision and it's not the first valid one, so just increment to the next spot
			else
				collision = collision->next;
		}
		return first;
	
	}