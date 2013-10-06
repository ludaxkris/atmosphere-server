#include "Base.h"

Base::Base(void)
{
	captureStartTime	= -1;
	lastAttackTime		= -1;
	attackerID			= -1;
	ownerID				= -1;
	radius				=  8.0f;
	ownerPresent		= false;
}