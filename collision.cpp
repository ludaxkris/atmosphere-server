#include "StdAfx.h"
#include ".\Collision.h"
#using <mscorlib.dll>

Collision::Collision(SpaceObject* object1, SpaceObject* object2)
{
	this->obj1 = object1;
	this->obj2 = object2;
	this->next = 0;
}

Collision::~Collision(void)
{
}
