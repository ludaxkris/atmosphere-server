#include "BaseManager.h"



BaseManager::BaseManager(ObjectManager * objManage) {
	objManager = objManage;
}


void BaseManager::update()
{	
  	int timeToCapture		= 4000;
	int timeBufferForLeave	= 500;

	//std::cerr<<"Base UPDATE"<<std::endl;

	//this will be similar...but we need an itemObject....
	for(int i=0; i< objManager->numBase; i++)
	{
		Base * curBase = &(objManager->privStartBase[i]);
		

		//clear owner present if it leaves area
		////////////////////////////////////////
		int lastOwnerVisitTime =  objManager->mary->currentTime - curBase->lastOwnerVisitTime;

		if(lastOwnerVisitTime > timeBufferForLeave && curBase->ownerPresent)
		{
			std::cerr<<"Owner::"<<curBase->ownerID<<"left base::"<<curBase->index<<std::endl;
			curBase->ownerPresent		= false;
			curBase->lastOwnerVisitTime	= -1;	
		}

		if(curBase->attackerID == -1)		
			continue;

		//Handle Capture
		///////////////////////////
		int captureTime = objManager->mary->currentTime - curBase->captureStartTime;	
		
		//std::cerr<<"Capture start time::"<<captureTime<<" for base::"<<curBase->index<<std::endl;

		if(captureTime > timeToCapture && curBase->attackerID != -1)
			swapBaseOwnership(curBase);

		//Clear attacker if it leaves area
		////////////////////////////////////////
		int lastAttackTime =  objManager->mary->currentTime - curBase->lastAttackTime;

		if(curBase->attackerID != -1  && lastAttackTime > timeBufferForLeave)
		{
			std::cerr<<"Attacker::"<<curBase->attackerID<<"left base::"<<curBase->index<<std::endl;
			curBase->attackerID			= -1;
			curBase->captureStartTime	= -1;
			curBase->lastAttackTime		= -1;
		}
	}
}


void BaseManager::doCollision(Base * base, PlayerShip * player)
{
	//check if its not the owner
	if(player->index != base->ownerID && !base->ownerPresent)
	{	
		//check for first collision only when owner is not present
		if(base->attackerID == -1)
		{
			std::cerr<<"Found first collision between player::"<<player->index;
			std::cerr<<"with base::"<<base->index<<std::endl;
			
			//set attacker flag and its capture start time
			base->attackerID = player->index;
			base->captureStartTime = objManager->mary->currentTime;

			//shared info flag
			((SharedInfoBase *)(base->sharedInfo))->baseFlag |= 1;

		}

		//check for collision update time
		if(base->attackerID == player->index)
		{
			//std::cerr<<"Updating collision between player::"<<player->index;
			//std::cerr<<"with base::"<<base->index<<std::endl;
			base->lastAttackTime = objManager->mary->currentTime;
		}
	}
	//if player is owner and if owner is not present
	else if(player->index == base->ownerID && base->attackerID == -1)
	{
		//if there's no attacker set owner present flag	
		{
			//debug
			if(!base->ownerPresent)
				std::cerr<<"Owner present at base"<<base->index<<std::endl;

			base->ownerPresent = true;
			base->lastOwnerVisitTime = objManager->mary->currentTime;
			
		}
	}
}


void BaseManager::swapBaseOwnership(Base * base) 
{	
	std::cerr<<"Player::"<<base->attackerID;
	std::cerr<<"Capture base::"<<base->index;
	std::cerr<<"Owner present"<<std::endl;

	//swap ownership
	base->ownerID = base->attackerID;
	((SharedInfoBase *)(base->sharedInfo))->playerID = base->attackerID;

	//reset flags
	base->attackerID		= -1;
	base->captureStartTime	= -1;
	base->lastAttackTime	= -1;

	//shared info flag
	((SharedInfoBase *)(base->sharedInfo))->baseFlag |= 2;

	//set that owner is present 
	base->ownerPresent = true;
	base->lastOwnerVisitTime = objManager->mary->currentTime;		
}


//Call this function if player ship and base are in vicinity.
void BaseManager::captureBase(Base * curBase)
{

}