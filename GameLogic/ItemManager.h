#pragma once


#include "./../Objects/ObjectManager.h"
#include "./../ConfigManager/ConfigManager.h"
#include <vector>


class ItemManager
{

public:
	ItemManager(ObjectManager * objManage);
	~ItemManager();

	void update();
	void respawnItem();
	void deleteItem(Item * curItem);
	void doCollision(Item * curItem, Ship * ship);


	ObjectManager * objManager;

	int respawnTime;

};