 #pragma once
#include "SpaceObject.h"

class Ship : public SpaceObject
{

public:

	//Everyone's stuff
	/////////////////////////////////////////
	Ship(void);
	////////////////////////////////////////

	//Gamelogic Stuff
	//////////////////////////////////////////
	int hitBy;
	int maxProjPerPlayer;
	int currentNumProj;	
	DWORD lastFired;
};
