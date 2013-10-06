#include "ItemManager.h"

using namespace std;

ItemManager::ItemManager(ObjectManager * objManage) {
	objManager = objManage;
}


/**
 *	Respawns items that were used after X amount of time has passed.
 */
void ItemManager::update() {

	int currTime = objManager->mary->currentTime;

	for(int i = 0 ; i < objManager->numItem; i++)
	{
		Item * curItem = &(objManager->privStartItem[i]);			

		//Handle Respawn
		///////////////////////////
		int takenTime = currTime - curItem->timeTaken;

		//Checking deleted items
		if( curItem->sharedInfo->next != -1 || curItem->sharedInfo->pre != -1)
		{
			if( takenTime > respawnTime )
			{
				respawnItem();
			}
		}		
		//else do nothing
	}
}


void ItemManager::respawnItem() {

	//Have this randomly generate different types of items.
	Item * curItem = objManager->createItem();
	((SharedInfoItem *)curItem->sharedInfo)->itemType = 1;
	((SharedInfoItem *)curItem->sharedInfo)->itemFlag |= (char)1;

	std::cerr<<"Item respawn::"<<curItem->index<<std::endl;

}

void ItemManager::deleteItem(Item * curItem) {
	std::cerr<<"Item taken::"<<curItem->index<<std::endl;

	((SharedInfoItem *)curItem->sharedInfo)->itemFlag |= (char)2;
	objManager->deleteItem(curItem);
	curItem->timeTaken = objManager->mary->currentTime;
}

void ItemManager::doCollision(Item * curItem, Ship * ship) {
		
	if(ship->isPlayerShip()) {
		cerr<<"PLAYER "<<ship->index<<" Captured "<< ((SharedInfoItem *)curItem->sharedInfo)->itemType<<endl;

		//determine which upgrade to give
		switch(((SharedInfoItem *)curItem->sharedInfo)->itemType) {

			case 30: {
				((SharedInfoShip*) ship->sharedInfo)->hp += 30;
				break;
			}
			default: {
				//do nothing
			}
		}

		deleteItem(curItem);	
	}
	//otherwise nothing happens
}
