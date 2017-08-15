#include "Projectile.h"

Projectile::Projectile()
{

}

Projectile::Projectile(ProjectileType Type)
{
	this->Type = Type;
}

Projectile::~Projectile()
{

}

void Projectile::Update(double dt)
{
	if (!CollisionCheck())
	{
		if (this->Type == BULLET)
		{
			
		}
		else
		{

		}
	}
	else
	{

	}
}

bool Projectile::CollisionCheck()
{
	return true;
}

void Projectile::CollisionResponse(double dt)
{
	
}