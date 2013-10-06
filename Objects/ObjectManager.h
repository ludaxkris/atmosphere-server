#pragma once

#include "SpaceObject.h"
#include "AIShip.h"
#include "Asteroid.h"
#include "PlayerShip.h"
#include "Base.h"
#include "Projectile.h"
#include "World.h"
#include "MaryMaid.h"
#include "Item.h"
#include "../ConfigManager/ConfigManager.h"
#include "../GraphicsManager/GraphicsManager.h"

class ObjectManager
{

protected:
	//config
	ConfigManager * cManager;
    ObjManagerConfig * config;
	
	//gManager
	GraphicsManager * gManager;

	SharedInfoPlayerShip* pubStartPlayerShip;
	SharedInfoAIShip* pubStartAIShip;
	SharedInfoAsteroid* pubStartAsteroid;
	SharedInfoBase*	pubStartBase;
	SharedInfoProjectile* pubStartProjectile;
	SharedInfoWorld* pubStartWorld;
	SharedInfoItem* pubStartItem;

	void InitPrivateMem();	// new
	void InitPublicMem();	// malloc
	void InitGraphics();	// graphics ie scaling

public:
	
	//Global
	int numProjectile;
	int numWorld;
	int numBase;
	int numPlayerShip;
	int numAIShip;
	int numAsteroid;
	int numItem;
	
	MaryMaid* mary;

	//Variables
	PlayerShip*		privStartPlayerShip;
	AIShip*			privStartAIShip;
	Asteroid*		privStartAsteroid;
	Base*			privStartBase;
	Projectile*		privStartProjectile;
	World*			privStartWorld;
	Item*			privStartItem;

	int totalSharedMemSize;

	SharedInfoSpaceObject*	m_PublicMem;

	//Functions
	ObjectManager();
	~ObjectManager();
	
	///////////NICKS GRAPHICS STUFF
	void* NICKS_GRAPHICS;


	//Accessor Functions
	PlayerShip* getPlayerShipList();
	AIShip*		getAIShipList();
	Asteroid*	getAsteroidList();
	Base*		getBaseList();
	Projectile* getProjectileList();
	World*		getWorldpList();
	Item*		getItemList();


	//create functions for server
	PlayerShip* createPlayerShip();
	PlayerShip* reCreatePlayerShip(int id);
	World*		createWorld();

	AIShip*		createAIShip();
	Asteroid*	createAsteroid();
	Base*		createBase();
	Projectile* createProjectile();
	Item*		createItem();


	//delete functions for server
	void		deletePlayerShip(PlayerShip* delShip);
	void		deleteWorld(World* delWorld);

	void		deleteAIShip(AIShip* delAIShip);
	void		deleteAsteroid(Asteroid* delAsteriod);
	void		deleteBase(Base* delBase);
	void		deleteProjectile(Projectile* delProjectile);
	void		deleteItem(Item* delItem);


};
