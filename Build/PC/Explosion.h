#pragma once
#include "Projectile.h"

//Michael Davis 26/02/2018

class Explosion : public Projectile {
public:
	Explosion();
	Explosion(Colour col, const Vector4& RGBA, Vector3 pos, Vector3 Velocity, float size, float inverseMass, PhysNodeType type, int projectileWorth, const std::string& name = "");

	virtual ~Explosion();

	bool ExplosionCallbackFunction(PhysicsNode* self, PhysicsNode* collidingObject);

protected:

};