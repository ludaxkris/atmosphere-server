#pragma once
#include "SpaceObject.h"

class Projectile : public SpaceObject
{

public:

	//Everyone's stuff
	//////////////////////////////////////////
	Projectile(void);
	/////////////////////////////////////////

	//Arjay's system stuff
	////////////////////////////////////////////
	D3DXVECTOR2 direction;
	virtual bool isProjectile(){return true;}
	////////////////////////////////////////////

	//Gamelogic Stuff
	//////////////////////////////////////////
	int createTime;
	int damageAmt;
};
