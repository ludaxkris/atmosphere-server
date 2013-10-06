#pragma once
#include "SpaceObject.h"

class Base : public SpaceObject
{

public:

	//Everyone's stuff
	///////////////////////////////////////////
	Base(void);
	////////////////////////////////////////////

	//Arjay's system stuff
	////////////////////////////////////////////
	virtual bool isBase(){return true;}
	////////////////////////////////////////////

	//Game Logic
	////////////////////////////////////////////
	int captureStartTime;
	int lastAttackTime;
	int lastOwnerVisitTime;
	
	bool ownerPresent;
	int attackerID;
	int ownerID;
};
