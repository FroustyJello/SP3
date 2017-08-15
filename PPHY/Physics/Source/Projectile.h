#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "GameObject.h"

enum ProjectileType
{
	BULLET,
	ARROW,
};

class Projectile : public GameObject
{
public:
	Projectile();
	Projectile(ProjectileType Type);
	~Projectile();

	void Update(double dt);
	bool CollisionCheck();
	void CollisionResponse(double dt);

private:
	ProjectileType Type;
};

#endif // !PROJECTILE_H