#include "./ProjectileManager.h"
#include <iostream>
using namespace std;
ProjectileManager::ProjectileManager(ObjectManager * objManage)
{
	objManager = objManage;
}

ProjectileManager::~ProjectileManager()
{

}
void ProjectileManager::doCollision (Projectile * proj, Ship * ship)
{
	//cerr<<"Player hit::"<<ship->index<<endl;
	//take hp off ship
	((SharedInfoShip*) ship->sharedInfo)->hp -= proj->damageAmt;
	ship->hitBy = (int)((SharedInfoProjectile *)proj->sharedInfo)->shipID;
		
	cerr<<"PLAYER "<<ship->index<<" GOT HIT BY "<< (int)((SharedInfoProjectile *)proj->sharedInfo)->shipID<<":"<<endl;
	cerr<<"PLAYER "<<ship->index<<" HP "<< (int)((SharedInfoShip *)ship->sharedInfo)->hp<<":"<<endl;
	deleteProjectile(proj);			
}

void ProjectileManager::createProjectile(PlayerShip* player, int type)
{	
	int currentTime = objManager->mary->currentTime;
	int timeElapsed = currentTime - player->lastFired; 
	
	//fire rate should be dependent on weapon type
	int fireRate		 = 80;

	//we can create projectile
	if( timeElapsed > fireRate && player->maxProjPerPlayer > player->currentNumProj)
	{		
		Projectile* proj = objManager->createProjectile();
				
		if (proj != NULL)
		{			
			SharedInfoProjectile* sharedInfo = (SharedInfoProjectile*)proj->sharedInfo;

			//increase proj number
			player->currentNumProj++;

			//look at type and init correct projectile
			proj->damageAmt = 5;			

			//set flag to just created
			((SharedInfoProjectile*)proj->sharedInfo)->projFlag |= (char)1; 
			sharedInfo->shipID = player->index;
			sharedInfo->type = 0;

			//set proj parameters for movement
			proj->initRotXY(player->getRotXY());
			proj->direction.x	= player->fireDirection.x;
			proj->direction.y	= player->fireDirection.y;		
			proj->createTime	= currentTime;					
		}
		else
		{
			cerr<<"Couldn't create Projectile."<<endl;
		}

		player->lastFired = currentTime;
	}

	//reset player fire rate
	player->fireDirection.x = 0;
	player->fireDirection.y = 0;	
}


void ProjectileManager::deleteProjectile(Projectile* proj)
{		
		objManager->deleteProjectile(proj);

		//decrement num proj per player
		objManager->privStartPlayerShip[((SharedInfoProjectile*)proj->sharedInfo)->shipID].currentNumProj--;

		//set delete flag
		((SharedInfoProjectile*)proj->sharedInfo)->projFlag |= (char)2; 

}


void ProjectileManager::update()
{
	int currentTime = objManager->mary->currentTime;

	//Handle Create Projectile
	///////////////////////////	
	for(int curLoc = objManager->mary->playerShipStart; 
		-1 != curLoc;
		curLoc = objManager->privStartPlayerShip[curLoc].sharedInfo->next)
	{
		PlayerShip* curShip = &(objManager->privStartPlayerShip[curLoc]);
				
		if(curShip->fireDirection.x || curShip->fireDirection.y)
			createProjectile(curShip, 0);
	}

	//Handle Projectile Life
	///////////////////////////	
	int lifeTime = 2000;
	std::vector<Projectile*> toDelete;		

	for(int curLoc = objManager->mary->projectileStart ; 
		-1 != curLoc ;
		curLoc = objManager->privStartProjectile[curLoc].sharedInfo->next)
	{
		Projectile* curProj = &(objManager->privStartProjectile[curLoc]);

		int age = currentTime - curProj->createTime;

		//check for alive time, if pass delete
		if(age >= lifeTime)
			toDelete.push_back(curProj);
		else
		{
			///update projectile movement
			curProj->UpdatePosition(curProj->direction);		
		}
	}

	//delete My Effects
	for(unsigned int i=0; i < toDelete.size(); i++)
	{		
		deleteProjectile(toDelete.at(i));
	}

}