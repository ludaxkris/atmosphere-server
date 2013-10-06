#pragma once


#include "./../Objects/ObjectManager.h"
#include "./../ConfigManager/ConfigManager.h"
#include <vector>


class PlayerManager
{

public:
	PlayerManager(ObjectManager * objManage);
	~PlayerManager();

	void disconnectPlayer(PlayerShip* player);

	void update();
	void movePlayer(PlayerShip * player);
	int checkPlayerScore(int winningScore);

	ObjectManager * objManager;

	int respawnTime;

};