#pragma once
#include "SpaceObject.h"

class World : public SpaceObject
{

public:

	//Everyone's
	//////////////////////////////////////////
	World(void);
	//////////////////////////////////////////

	//Arjay's system stuff
	////////////////////////////////////////////
	virtual bool isWorld(){return true;}
	////////////////////////////////////////////
};