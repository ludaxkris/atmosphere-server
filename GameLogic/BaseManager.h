#pragma once


#include "./../Objects/ObjectManager.h"
#include "./../ConfigManager/ConfigManager.h"
#include <vector>


class BaseManager
{

public:
	BaseManager(ObjectManager * objManage);
	~BaseManager();

	void update();
	void swapBaseOwnership(Base * base); 
	void captureBase(Base * curBase);
	void doCollision(Base * base, PlayerShip * player);

	ObjectManager * objManager;

	int captureTime;

};