#include "Particle.h"

ParticleObject::ParticleObject(ParticleObject_TYPE typeValue) :
	type(typeValue),
	pos(0.0f, 0.0f, 0.0f),
	scale(1.0f, 1.0f, 1.0f),
	vel(0.0f, 0.0f, 0.0f),
	rotation(0.0f),
	rotationSpeed(0.0f),
	isActive(false)
{
}

ParticleObject::~ParticleObject(void)
{
}
