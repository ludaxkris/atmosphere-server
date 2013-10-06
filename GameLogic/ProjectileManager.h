#pragma once


#include "../Objects/ObjectManager.h"
#include <vector>

class ProjectileManager
{
	public:
		ProjectileManager(ObjectManager * objManage);
		~ProjectileManager();

		void update();
		void createProjectile(PlayerShip* player, int type);

		void deleteProjectile(Projectile* proj);
		void doCollision (Projectile * proj, Ship * ship);
		ObjectManager * objManager;
};

