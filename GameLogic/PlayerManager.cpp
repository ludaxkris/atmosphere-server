#include "PlayerManager.h"
#include <iostream>

using namespace std;

PlayerManager::PlayerManager(ObjectManager * objManage) {
	objManager	= objManage;
	respawnTime = 5000;
}

void PlayerManager::disconnectPlayer(PlayerShip* player)
{
	std::cerr<<"REMOVEING PLAYER FOR DISCONNECT::"<<player->index<<std::endl;
	objManager->deletePlayerShip(player);
	((SharedInfoPlayerShip *)(player->sharedInfo))->hp = -1;
	player->dieTime = -1;
	
}
void PlayerManager::update() {	

	int currTime = objManager->mary->currentTime;

	for(int i = 0 ; i < 4; i++)
	{
		PlayerShip * curPlayer = &(objManager->privStartPlayerShip[i]);
			

		//Handle Death
		///////////////////////////
		int deathTime = currTime - curPlayer->dieTime;

		if( ((SharedInfoPlayerShip *)curPlayer->sharedInfo)->hp <= 0 && curPlayer->dieTime >= 0)			 
		{					
			//std::cerr<<"Player ::"<<curPlayer->index<<" has health::"<<((SharedInfoPlayerShip *)curPlayer->sharedInfo)->hp<<" DIE::"<<curPlayer->dieTime<<std::endl;
			//warning check for alive will not work with only 1 player
			if( curPlayer->sharedInfo->next != -1 || curPlayer->sharedInfo->pre != -1)
			{
				std::cerr<<"Player killed::"<<curPlayer->index<<std::endl;
				objManager->deletePlayerShip(curPlayer);
				curPlayer->dieTime = currTime;

				//Assign Point to killer
				objManager->privStartPlayerShip[curPlayer->hitBy].score += 1;
				std::cerr<<"Player ::"<<curPlayer->index<<" has score::"<<objManager->privStartPlayerShip[curPlayer->hitBy].score<<std::endl;
			//warning check for alive will not work with only 1 player


			}
			//respawn			
			else if(deathTime > respawnTime )
			{
				std::cerr<<"Player respawn::"<<curPlayer->index<<std::endl;
				((SharedInfoPlayerShip *)curPlayer->sharedInfo)->hp = 100;
				objManager->reCreatePlayerShip(i);
			}
				//other dead functions
		}
		//Move Player
		///////////////////////////
		else {
			movePlayer(curPlayer);
		}
	}
}

void PlayerManager::movePlayer(PlayerShip * player) {
	if(player->movement.x || player->movement.y)
	{
		player->UpdatePosition(player->movement);			
		player->movement.x = 0;
		player->movement.y = 0;
	}	
	//else player not moving
}

int PlayerManager::checkPlayerScore(int winningScore) {
	for(int i = 0 ; i < 4; i++)
	{
		PlayerShip * curPlayer = &(objManager->privStartPlayerShip[i]);
		if(curPlayer->score >= winningScore)
			return i;
	}

	//No winners yet
	return -1;
}

