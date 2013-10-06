#pragma once
#include "./Objects/SpaceObject.h"

class Collision
{
public:
	SpaceObject* obj1;
	SpaceObject* obj2;
	Collision* next;

	Collision(SpaceObject* object1, SpaceObject* object2);
	~Collision(void);
};
