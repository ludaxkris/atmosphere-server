#include "Projectile.h"
#include <stdio.h>

Projectile::Projectile(void)
{
	this->radius = .0174f;
	createTime	= 0;
	direction.x = 0;
	direction.y = 0;
}