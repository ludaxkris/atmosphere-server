#include "ObjectManager.h"
#include "ObjectManager.h"
#include <iostream>


//#define numWorld 1
//#define numBase 16
//#define numPlayerShip 4
//#define numAIShip 15
//#define numAsteroid 15
//#define numProjectile 15

using namespace std;

//================================
//Class::ObjectManager
//================================
ObjectManager::ObjectManager()
{
	numProjectile	= 80;
	numWorld		= 1;
	numBase			= 7;
	numPlayerShip	= 4;
	numAIShip		= 14;
	numAsteroid		= 15;
	numItem			= 7;


	//NICKS_GRAPHICS = NG;
	//config manager
	cManager = new ConfigManager();
    config = cManager->buildObjManager();

	totalSharedMemSize = 0;

	InitPublicMem();
	InitPrivateMem();
	InitGraphics();
	

}


ObjectManager::~ObjectManager()
{
	delete privStartPlayerShip;
	delete privStartAIShip;
	delete privStartAsteroid;
	delete privStartBase;
	delete privStartProjectile;
	delete privStartWorld;
	delete privStartItem;
	privStartPlayerShip = NULL;
	privStartAIShip = NULL;
	privStartAsteroid = NULL;
	privStartBase = NULL;
	privStartProjectile = NULL;
	privStartWorld = NULL;
	privStartItem = NULL;
	free(m_PublicMem);
}


void ObjectManager::InitGraphics()
{
	privStartWorld[0].initObject(config->worldScaleX, 0);
	//privStartWorld[0].mesh = gManager->getMesh(config->worldMesh);

	//init player ships
	for(int i = 0; i< numPlayerShip; i++)
	{
		//privStartPlayerShip[i].initObject(.05f, -22);
		privStartPlayerShip[i].initObject(config->players.at(i)->scaleX, config->players.at(i)->position.z);
	}
	
	//init projectiles
	for(int i = 0; i< numProjectile; i++)
	{	
		//privStartProjectile[i].initObject(.01f, -22); 
		privStartProjectile[i].initObject(.01f, config->players.at(0)->position.z); 
	}

	//init bases
	for(int i = 0; i< numBase; i++)
	{	
		//privStartBase[i].initObject(.05f, -22); 
		privStartBase[i].initObject(config->bases.at(i)->scaleX, config->bases.at(i)->position.z);
		//privStartBase[i].initObject(.05f, -22, D3DXVECTOR3(1,0,0)); 

	}

	/////////////////////// FILL IN!! /////////////////////////
	//init items
	for(int i = 0; i< numItem; i++)
	{	
		//privStartItem[i].initObject(.05f, -22); 
		//privStartItem[i].initObject(config->bases.at(i)->scaleX, config->bases.at(i)->position.z);
		privStartItem[i].initObject(.05f, -22, D3DXVECTOR3(1,0,0)); 

	}
}

void ObjectManager::InitPrivateMem()
{
	//init all mary's stuff
	mary->AIShipStart = -1;
	mary->asteroidStart = -1;
	mary->baseStart = -1;
	mary->playerShipStart = -1;
	mary->projectileStart = -1;
	mary->worldStart = -1;
	mary->itemStart = -1;
	mary->currentTime = timeGetTime(); 

	privStartPlayerShip = new PlayerShip [numPlayerShip];

	//assign all the priv ships to pub ships
	for(int i = 0; i< numPlayerShip; i++)
	{
		privStartPlayerShip[i].sharedInfo =
			(SharedInfoPlayerShip *)((char *)pubStartPlayerShip + ( i* sizeof(SharedInfoPlayerShip)));
		privStartPlayerShip[i].index = i;
		privStartPlayerShip[i].sharedInfo->next = -1;
		privStartPlayerShip[i].sharedInfo->pre	= -1;
		privStartPlayerShip[i].dieTime			= -1;
		privStartPlayerShip[i].score			= 0;
		privStartPlayerShip[i].maxProjPerPlayer = 20;
		((SharedInfoPlayerShip*)privStartPlayerShip[i].sharedInfo)->playerFlag = 0;
	}

	privStartAIShip = new AIShip [numAIShip];
	//assign all the priv AI ships to pub ships
	for(int i = 0; i< numAIShip; i++)
	{
		privStartAIShip[i].sharedInfo =
			(SharedInfoAIShip *)((char *)pubStartAIShip + ( i* sizeof(SharedInfoAIShip)));
		privStartAIShip[i].index			= i;
		privStartAIShip[i].sharedInfo->next = -1;
		privStartAIShip[i].sharedInfo->pre	= -1;
		privStartAIShip[i].maxProjPerPlayer = 10;
	}


	privStartAsteroid = new Asteroid [numAsteroid];
	//assign all the priv ships to pub ships
	for(int i = 0; i<numAsteroid ; i++)
	{
		privStartAsteroid[i].sharedInfo =
			(SharedInfoAsteroid *)((char *)pubStartAsteroid + ( i* sizeof(SharedInfoAsteroid)));
		privStartAsteroid[i].index = i;
		privStartAsteroid[i].sharedInfo->next = -1;
		privStartAsteroid[i].sharedInfo->pre = -1;
	}

	privStartBase = new Base [numBase];
	//assign all the priv ships to pub ships
	for(int i = 0; i< numBase; i++)
	{
		privStartBase[i].sharedInfo =
			(SharedInfoBase *)((char *)pubStartBase + ( i* sizeof(SharedInfoBase)));
		privStartBase[i].index = i;
		privStartBase[i].sharedInfo->next = -1;
		privStartBase[i].sharedInfo->pre = -1;
	}

	privStartProjectile = new Projectile [numProjectile];
	//assign all the priv ships to pub ships
	for(int i = 0; i< numProjectile; i++)
	{
		privStartProjectile[i].sharedInfo =
			(SharedInfoProjectile *)((char *)pubStartProjectile + ( i* sizeof(SharedInfoProjectile)));
		privStartProjectile[i].index = i;
		privStartProjectile[i].sharedInfo->next = -1;
		privStartProjectile[i].sharedInfo->pre = -1;
		((SharedInfoProjectile*)(privStartProjectile[i].sharedInfo))->projFlag = 0;
	}

	privStartWorld = new World [numWorld];
	//assign all the priv ships to pub ships
	for(int i = 0; i<numWorld ; i++)
	{
		privStartWorld[i].sharedInfo =
			(SharedInfoWorld *)((char *)pubStartWorld + ( i* sizeof(SharedInfoWorld)));
		privStartWorld[i].index = i;
		privStartWorld[i].sharedInfo->next = -1;
		privStartWorld[i].sharedInfo->pre = -1;
	}

	/////////////////////// FILL IN!! /////////////////////////
	privStartItem = new Item [numItem];
	//assign all the priv ships to pub ships
	for(int i = 0; i<numItem ; i++)
	{
		privStartItem[i].sharedInfo =
			(SharedInfoItem *)((char *)pubStartItem + ( i* sizeof(SharedInfoItem)));
		privStartItem[i].index = i;
		privStartItem[i].sharedInfo->next = -1;
		privStartItem[i].sharedInfo->pre = -1;
	}


	//init world in game ctr because there's only gonna be one world
	mary->worldStart = 0;

}	

PlayerShip* ObjectManager::createPlayerShip()
{
	//if there is no ship created, create one by linking the first item
	if(mary->playerShipStart == -1)
	{
		mary->playerShipStart = 0;
		privStartPlayerShip[mary->playerShipStart].sharedInfo->next = -1;
		privStartPlayerShip[mary->playerShipStart].sharedInfo->pre = -1;
	}
	//else hook one up to the start of the list
	else
	{
		
		//i = next avalible playership to be taken
		int i;
		//find next free playership spot
		for(i = 0; i<numPlayerShip; i++) {
			//the playership should not be connected (next = pre = -1)
			if(privStartPlayerShip[i].sharedInfo->next == -1 && privStartPlayerShip[i].sharedInfo->pre == -1)
				break;
		}

		//this means that there is only one node and it's the first node and it's node 0
		if(mary->playerShipStart == i)
			i++;

		//save old location
		int old = mary->playerShipStart;

		//insert the new playership at the start of the list
		privStartPlayerShip[i].sharedInfo->next = old;
		privStartPlayerShip[i].sharedInfo->pre = -1;

		privStartPlayerShip[old].sharedInfo->pre = i;
		mary->playerShipStart = i;
		//inserted at start, return start
				
		//find last spot		
	}
	
	//set create flag for player
	((SharedInfoPlayerShip*)privStartPlayerShip[mary->playerShipStart].sharedInfo)->playerFlag |= 1;

	return &privStartPlayerShip[mary->playerShipStart];
}


PlayerShip* ObjectManager::reCreatePlayerShip(int id)
{
	//if there is no ship created, create one by linking the first item
	if(mary->playerShipStart == -1)
	{
		mary->playerShipStart = id;
		privStartPlayerShip[mary->playerShipStart].sharedInfo->next = -1;
		privStartPlayerShip[mary->playerShipStart].sharedInfo->pre = -1;
	}
	//else hook one up to the start of the list
	else
	{
		
		//i = next avalible playership to be taken
		int i = id;


		//save old location
		int old = mary->playerShipStart;

		//insert the new playership at the start of the list
		privStartPlayerShip[i].sharedInfo->next = old;
		privStartPlayerShip[i].sharedInfo->pre = -1;
		privStartPlayerShip[old].sharedInfo->pre = i;		
		mary->playerShipStart = i;

		//inserted at start, return start
				
		//find last spot		
	}

	//set create flag for player
	((SharedInfoPlayerShip*)privStartPlayerShip[mary->playerShipStart].sharedInfo)->playerFlag |= 1;

	//restore hp
	((SharedInfoPlayerShip*)privStartPlayerShip[mary->playerShipStart].sharedInfo)->hp = 100;

	//restore dieTime
	privStartPlayerShip[mary->playerShipStart].dieTime = 0;

	//restore score

	return &privStartPlayerShip[mary->playerShipStart];
}

World* ObjectManager::createWorld()
{
	//if there is no world created, create one by linking the first item
	if(mary->worldStart == -1)
	{
		mary->worldStart = 0;

		privStartWorld[mary->worldStart].sharedInfo->next = -1;
		privStartWorld[mary->worldStart].sharedInfo->pre = -1;
		return &privStartWorld[mary->worldStart];
	}
	//else hook one up to the start of the list
	else
	{
		//i = next avalible playership to be taken
		int i;
		//find next free playership spot
		for(i = 0; i<numWorld; i++) {
			//the playership should not be connected (next = pre = -1)
			if(privStartWorld[i].sharedInfo->next == -1 && privStartWorld[i].sharedInfo->pre == -1)
				break;
		}
		
		//this means that there is only one node and it's the first node and it's node 0
		if(mary->worldStart == i)
			i++;

		//save old location
		int old = mary->worldStart;

		//insert the new playership at the start of the list
		privStartWorld[i].sharedInfo->next = old;
		privStartWorld[i].sharedInfo->pre = -1;

		privStartWorld[old].sharedInfo->pre = i;
		mary->worldStart = i;
		return	&privStartWorld[mary->worldStart];
	}
}
AIShip*		ObjectManager::createAIShip()
{
	//if there is no world created, create one by linking the first item
	if(mary->AIShipStart == -1)
	{
		mary->AIShipStart = 0;

		privStartAIShip[mary->AIShipStart].sharedInfo->next = -1;
		privStartAIShip[mary->AIShipStart].sharedInfo->pre = -1;
		return &privStartAIShip[mary->AIShipStart];
	}
	//else hook one up to the start of the list
	else
	{
		//i = next avalible playership to be taken
		int i;
		//find next free playership spot
		for(i = 0; i<numAIShip; i++) {
			//the playership should not be connected (next = pre = -1)
			if(privStartAIShip[i].sharedInfo->next == -1 && privStartAIShip[i].sharedInfo->pre == -1)
				break;
		}

		//this means that there is only one node and it's the first node and it's node 0
		if(mary->AIShipStart == i)
			i++;


		//save old location
		int old = mary->AIShipStart;

		//insert the new playership at the start of the list
		privStartAIShip[i].sharedInfo->next = old;
		privStartAIShip[i].sharedInfo->pre = -1;

		privStartAIShip[old].sharedInfo->pre = i;
		mary->AIShipStart = i;


		return	&privStartAIShip[mary->AIShipStart];
	}
}
Asteroid*	ObjectManager::createAsteroid()
{
	//if there is no world created, create one by linking the first item
	if(mary->asteroidStart == -1)
	{
		mary->asteroidStart = 0;

		privStartAsteroid[mary->asteroidStart].sharedInfo->next = -1;
		privStartAsteroid[mary->asteroidStart].sharedInfo->pre = -1;
		return &privStartAsteroid[mary->asteroidStart];
	}
	//else hook one up to the start of the list
	else
	{
		//i = next avalible playership to be taken
		int i;
		//find next free playership spot
		for(i = 0; i<numAsteroid ; i++) {
			//the playership should not be connected (next = pre = -1)
			if(privStartAsteroid[i].sharedInfo->next == -1 && privStartAsteroid[i].sharedInfo->pre == -1)
				break;
		}

		//this means that there is only one node and it's the first node and it's node 0
		if(mary->asteroidStart == i)
			i++;

		//save old location
		int old = mary->asteroidStart;

		//insert the new playership at the start of the list
		privStartAsteroid[i].sharedInfo->next = old;
		privStartAsteroid[i].sharedInfo->pre = -1;

		privStartAsteroid[old].sharedInfo->pre = i;
		mary->asteroidStart = i;

		return	&privStartAsteroid[mary->asteroidStart];
	}
}


Base*		ObjectManager::createBase()
{
	//if there is no world created, create one by linking the first item
	if(mary->baseStart == -1)
	{
		mary->baseStart = 0;

		privStartBase[mary->baseStart].sharedInfo->next = -1;
		privStartBase[mary->baseStart].sharedInfo->pre = -1;
		return &privStartBase[mary->baseStart];
	}
	//else hook one up to the start of the list
	else
	{
		//i = next avalible playership to be taken
		int i;
		//find next free playership spot
		for(i = 0; i<numBase; i++) {
			//the playership should not be connected (next = pre = -1)
			if(privStartBase[i].sharedInfo->next == -1 && privStartBase[i].sharedInfo->pre == -1)
				break;
		}

		//this means that there is only one node and it's the first node and it's node 0
		if(mary->baseStart == i)
			i++;

		//save old location
		int old = mary->baseStart;

		//insert the new playership at the start of the list
		privStartBase[i].sharedInfo->next = old;
		privStartBase[i].sharedInfo->pre = -1;

		privStartBase[old].sharedInfo->pre = i;
		mary->baseStart = i;
		return	&privStartBase[mary->baseStart];
	}
}

Projectile* ObjectManager::createProjectile()
{
	//cerr<<"\n\nMary Proj start::"<<mary->projectileStart<<endl;
	
	//if there is no world created, create one by linking the first item
	if(mary->projectileStart == -1)
	{
		mary->projectileStart = 0;

		privStartProjectile[mary->projectileStart].sharedInfo->next = -1;
		privStartProjectile[mary->projectileStart].sharedInfo->pre = -1;
		return &privStartProjectile[mary->projectileStart];
	}
	//else hook one up to the start of the list
	else
	{
		//i = next avalible playership to be taken
		int i;
		//find next free playership spot
		for(i = 0; i<numProjectile; i++) {
			//the playership should not be connected (next = pre = -1)
			//cerr<<"Checking Proj::"<<i<<"Next::"<<privStartProjectile[i].sharedInfo->next<<"Pre::"<<privStartProjectile[i].sharedInfo->pre<<endl;
			if(privStartProjectile[i].sharedInfo->next == -1 && privStartProjectile[i].sharedInfo->pre == -1 && 
				mary->projectileStart != i)
				break;
		}
		
		if(i >= numProjectile)
			return NULL;

		//save old location
		int old = mary->projectileStart;

		//insert the new playership at the start of the list
		privStartProjectile[i].sharedInfo->next = old;
		privStartProjectile[i].sharedInfo->pre = -1;

		privStartProjectile[old].sharedInfo->pre = i;
		mary->projectileStart = i;

		cerr<<"Projectile create::"<<mary->projectileStart<<endl;
		return	&privStartProjectile[mary->projectileStart];
	}
}

Item* ObjectManager::createItem()
{
	//cerr<<"\n\nMary Item start::"<<mary->itemStart<<endl;
	
	//if there is no world created, create one by linking the first item
	if(mary->itemStart == -1)
	{
		mary->itemStart = 0;

		privStartItem[mary->itemStart].sharedInfo->next = -1;
		privStartItem[mary->itemStart].sharedInfo->pre = -1;
		return &privStartItem[mary->itemStart];
	}
	//else hook one up to the start of the list
	else
	{
		//i = next avalible playership to be taken
		int i;
		//find next free playership spot
		for(i = 0; i<numItem; i++) {
			//the playership should not be connected (next = pre = -1)
			//cerr<<"Checking Item::"<<i<<"Next::"<<privStartItem[i].sharedInfo->next<<"Pre::"<<privStartItem[i].sharedInfo->pre<<endl;
			if(privStartItem[i].sharedInfo->next == -1 && privStartItem[i].sharedInfo->pre == -1 && 
				mary->itemStart != i)
				break;
		}
		
		if(i >= numItem)
			return NULL;

		//save old location
		int old = mary->itemStart;

		//insert the new playership at the start of the list
		privStartItem[i].sharedInfo->next = old;
		privStartItem[i].sharedInfo->pre = -1;

		privStartItem[old].sharedInfo->pre = i;
		mary->itemStart = i;

		cerr<<"Item create::"<<mary->itemStart<<endl;
		return	&privStartItem[mary->itemStart];
	}
}


void ObjectManager::deletePlayerShip(PlayerShip* delShip)
{
	//if no other ship in list
	if(delShip->sharedInfo->next == -1 && delShip->sharedInfo->pre == -1)
		mary->playerShipStart = -1;

	//if next doesn't exist
	else if(delShip->sharedInfo->next == -1)
	{
		privStartPlayerShip[delShip->sharedInfo->pre].sharedInfo->next = -1;
	}

	//if pre doesn't exist
	else if(delShip->sharedInfo->pre == -1)
	{	
		//if we remove the root, reconnect
		if(mary->playerShipStart == delShip->index)
			mary->playerShipStart = delShip->sharedInfo->next;

		privStartPlayerShip[delShip->sharedInfo->next].sharedInfo->pre = -1;		
	}
	//both links are valid
	else
	{
		privStartPlayerShip[delShip->sharedInfo->pre].sharedInfo->next = delShip->sharedInfo->next;
		privStartPlayerShip[delShip->sharedInfo->next].sharedInfo->pre = delShip->sharedInfo->pre;
	}


	delShip->sharedInfo->pre = -1;
	delShip->sharedInfo->next = -1;
	((SharedInfoPlayerShip*)delShip->sharedInfo)->playerFlag |= 2;

	//set life to -1
}


void ObjectManager::deleteWorld(World* delWorld)
{
	//if no other ship in list
	if(delWorld->sharedInfo->next == -1 && delWorld->sharedInfo->pre == -1)
		mary->worldStart = -1;

	//if next doesn't exist
	else if(delWorld->sharedInfo->next == -1)
	{
		privStartWorld[delWorld->sharedInfo->pre].sharedInfo->next = -1;
		//disconnect the ship from the list
		delWorld->sharedInfo->pre = -1;
	}

	//if pre doesn't exist
	else if(delWorld->sharedInfo->pre == -1)
	{
		privStartWorld[delWorld->sharedInfo->next].sharedInfo->pre = -1;
		//disconnect the ship from the list
		delWorld->sharedInfo->next = -1;
	}
	//both links are valid
	else
	{
		privStartWorld[delWorld->sharedInfo->pre].sharedInfo->next = delWorld->sharedInfo->next;
		privStartWorld[delWorld->sharedInfo->next].sharedInfo->pre = delWorld->sharedInfo->pre;
		delWorld->sharedInfo->pre = -1;
		delWorld->sharedInfo->next = -1;
	}
}
void		ObjectManager::deleteAIShip(AIShip* delAIShip)
{
	//if no other ship in list
	if(delAIShip->sharedInfo->next == -1 && delAIShip->sharedInfo->pre == -1)
		mary->AIShipStart = -1;

	//if next doesn't exist
	else if(delAIShip->sharedInfo->next == -1)
	{
		privStartAIShip[delAIShip->sharedInfo->pre].sharedInfo->next = -1;
		//disconnect the ship from the list
		delAIShip->sharedInfo->pre = -1;
	}

	//if pre doesn't exist
	else if(delAIShip->sharedInfo->pre == -1)
	{
		privStartAIShip[delAIShip->sharedInfo->next].sharedInfo->pre = -1;
		//disconnect the ship from the list
		delAIShip->sharedInfo->next = -1;
	}
	//both links are valid
	else
	{
		privStartAIShip[delAIShip->sharedInfo->pre].sharedInfo->next = delAIShip->sharedInfo->next;
		privStartAIShip[delAIShip->sharedInfo->next].sharedInfo->pre = delAIShip->sharedInfo->pre;
		delAIShip->sharedInfo->pre = -1;
		delAIShip->sharedInfo->next = -1;
	}
}

void		ObjectManager::deleteAsteroid(Asteroid* delAsteriod)
{
	//if no other ship in list
	if(delAsteriod->sharedInfo->next == -1 && delAsteriod->sharedInfo->pre == -1)
		mary->asteroidStart = -1;

	//if next doesn't exist
	else if(delAsteriod->sharedInfo->next == -1)
	{
		privStartAsteroid[delAsteriod->sharedInfo->pre].sharedInfo->next = -1;
		//disconnect the ship from the list
		delAsteriod->sharedInfo->pre = -1;
	}

	//if pre doesn't exist
	else if(delAsteriod->sharedInfo->pre == -1)
	{
		privStartAsteroid[delAsteriod->sharedInfo->next].sharedInfo->pre = -1;
		//disconnect the ship from the list
		delAsteriod->sharedInfo->next = -1;
	}
	//both links are valid
	else
	{
		privStartAsteroid[delAsteriod->sharedInfo->pre].sharedInfo->next = delAsteriod->sharedInfo->next;
		privStartAsteroid[delAsteriod->sharedInfo->next].sharedInfo->pre = delAsteriod->sharedInfo->pre;
		delAsteriod->sharedInfo->pre = -1;
		delAsteriod->sharedInfo->next = -1;
	}
}

void		ObjectManager::deleteBase(Base* delBase)
{
	//if no other ship in list
	if(delBase->sharedInfo->next == -1 && delBase->sharedInfo->pre == -1)
		mary->worldStart = -1;

	//if next doesn't exist
	else if(delBase->sharedInfo->next == -1)
	{
		privStartBase[delBase->sharedInfo->pre].sharedInfo->next = -1;
		//disconnect the ship from the list
		delBase->sharedInfo->pre = -1;
	}

	//if pre doesn't exist
	else if(delBase->sharedInfo->pre == -1)
	{
		privStartBase[delBase->sharedInfo->next].sharedInfo->pre = -1;
		//disconnect the ship from the list
		delBase->sharedInfo->next = -1;
	}
	//both links are valid
	else
	{
		privStartBase[delBase->sharedInfo->pre].sharedInfo->next = delBase->sharedInfo->next;
		privStartBase[delBase->sharedInfo->next].sharedInfo->pre = delBase->sharedInfo->pre;
		delBase->sharedInfo->pre = -1;
		delBase->sharedInfo->next = -1;
	}
}

void		ObjectManager::deleteProjectile(Projectile* delProjectile)
{

	//check for invalid delete
	if(delProjectile->sharedInfo->next == -1 && delProjectile->sharedInfo->pre == -1 &&
		mary->projectileStart != delProjectile->index)
	{
		cerr<<"ERROR DELETE PROJECTILE::"<<delProjectile->index<<endl;
		return;
	}

	//if no other projectile in list
	if(delProjectile->sharedInfo->next == -1 && delProjectile->sharedInfo->pre == -1)
	{
		mary->projectileStart = -1;
	}

	//if next doesn't exist
	else if(delProjectile->sharedInfo->next == -1)
	{
		privStartProjectile[delProjectile->sharedInfo->pre].sharedInfo->next = -1;
		//disconnect the ship from the list
		//delProjectile->sharedInfo->pre = -1;
	}

	//if pre doesn't exist
	else if(delProjectile->sharedInfo->pre == -1)
	{
		//if we remove the root, reconnect
		if(mary->projectileStart == delProjectile->index)
			mary->projectileStart = delProjectile->sharedInfo->next;

		privStartProjectile[delProjectile->sharedInfo->next].sharedInfo->pre = -1;
		
	}
	//both links are valid
	else
	{
		privStartProjectile[delProjectile->sharedInfo->pre].sharedInfo->next = delProjectile->sharedInfo->next;
		privStartProjectile[delProjectile->sharedInfo->next].sharedInfo->pre = delProjectile->sharedInfo->pre;
	}

	delProjectile->sharedInfo->pre = -1;
	delProjectile->sharedInfo->next = -1;
}

void		ObjectManager::deleteItem(Item* delItem)
{

	//check for invalid delete
	if(delItem->sharedInfo->next == -1 && delItem->sharedInfo->pre == -1 &&
		mary->itemStart != delItem->index)
	{
		cerr<<"ERROR DELETE Item::"<<delItem->index<<endl;
		return;
	}

	//if no other item in list
	if(delItem->sharedInfo->next == -1 && delItem->sharedInfo->pre == -1)
	{
		mary->itemStart = -1;
	}

	//if next doesn't exist
	else if(delItem->sharedInfo->next == -1)
	{
		privStartItem[delItem->sharedInfo->pre].sharedInfo->next = -1;
		//disconnect the ship from the list
		//delItem->sharedInfo->pre = -1;
	}

	//if pre doesn't exist
	else if(delItem->sharedInfo->pre == -1)
	{
		//if we remove the root, reconnect
		if(mary->itemStart == delItem->index)
			mary->itemStart = delItem->sharedInfo->next;

		privStartItem[delItem->sharedInfo->next].sharedInfo->pre = -1;
		
	}
	//both links are valid
	else
	{
		privStartItem[delItem->sharedInfo->pre].sharedInfo->next = delItem->sharedInfo->next;
		privStartItem[delItem->sharedInfo->next].sharedInfo->pre = delItem->sharedInfo->pre;
	}

	delItem->sharedInfo->pre = -1;
	delItem->sharedInfo->next = -1;
}

void ObjectManager::InitPublicMem()
{

		totalSharedMemSize =							  sizeof(MaryMaid) 		
														+ (sizeof(SharedInfoWorld) * numWorld)
														+ (sizeof(SharedInfoBase) * numBase)
														+ (sizeof(SharedInfoPlayerShip)* numPlayerShip)
														+ (sizeof(SharedInfoAIShip)* numAIShip)		
														+ (sizeof(SharedInfoAsteroid)* numAsteroid)		
														+ (sizeof(SharedInfoProjectile)* numProjectile)														
														+ (sizeof(SharedInfoItem)* numItem);



		m_PublicMem = (SharedInfoSpaceObject *)malloc (totalSharedMemSize);

		//mary had a little lamb, it's fleece was white as snow and everywhere that mary went, her sheep was sure to go
		//casting to (char*) because we need to add the address and not by its scale, the size of its content.
		// ie.  int * a; a+1 inceases the address in a by 4 not 1.
		mary				= (MaryMaid*) m_PublicMem;
		pubStartWorld		= (SharedInfoWorld*)((char*)mary+ sizeof(MaryMaid));
		pubStartBase		= (SharedInfoBase*)((char*)pubStartWorld +  sizeof(SharedInfoWorld) * numWorld);	
		pubStartPlayerShip  = (SharedInfoPlayerShip*)((char*)pubStartBase + sizeof(SharedInfoBase) * numBase);
		pubStartAIShip		= (SharedInfoAIShip*)((char*)pubStartPlayerShip  + sizeof(SharedInfoPlayerShip)* numPlayerShip);
		pubStartAsteroid	= (SharedInfoAsteroid*)((char*)pubStartAIShip + sizeof(SharedInfoAIShip)* numAIShip);		
		pubStartProjectile	= (SharedInfoProjectile*)((char*)pubStartAsteroid + sizeof(SharedInfoAsteroid)* numAsteroid);	
		pubStartItem		= (SharedInfoItem*)((char*)pubStartProjectile + sizeof(SharedInfoProjectile)* numProjectile);	

		memset(m_PublicMem, 0, totalSharedMemSize);
}





//Accessor functions
//////////////////////////////////
PlayerShip* ObjectManager::getPlayerShipList()
{
	return &privStartPlayerShip[mary ->playerShipStart];
}

AIShip* ObjectManager::getAIShipList()
{
	return &privStartAIShip[mary->AIShipStart];
}

Asteroid* ObjectManager::getAsteroidList()
{
	return &privStartAsteroid[mary ->asteroidStart];
}

Base* ObjectManager::getBaseList()
{
	return &privStartBase[mary->baseStart];
}

Projectile* ObjectManager::getProjectileList()
{
	return &privStartProjectile[mary ->projectileStart];
}

World* ObjectManager::getWorldpList()
{
	return &privStartWorld[mary ->worldStart];
}

Item* ObjectManager::getItemList()
{
	return &privStartItem[mary ->itemStart];
}