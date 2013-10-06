#pragma once
#include "Ship.h"

class PlayerShip : public Ship
{

public:

	//Everyone's stuff
	///////////////////////////////////////////
	PlayerShip(void);
	///////////////////////////////////////////

	//Arjay's system stuff
	////////////////////////////////////////////
	DWORD timeLastFired; //in miliseconds 
	D3DXVECTOR2 movement;
	D3DXVECTOR2 fireDirection;
	virtual bool isPlayerShip(){return true;}
	////////////////////////////////////////////

	//Gamelogic Stuff
	//////////////////////////////////////////
	int dieTime;
	int score;
};
