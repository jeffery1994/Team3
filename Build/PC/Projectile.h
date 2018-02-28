#pragma once
//Michael Davis 14/02/2018

#include <ncltech/GameObject.h>
#include "Gameplay.h"
#include <ncltech/SceneManager.h>
#include <ncltech\SphereCollisionShape.h>
#include <ncltech\CuboidCollisionShape.h>
#include "Game.h"
#include "Minion.h"


class Projectile : public GameObject {
protected:
	Colour colour;
	int projectileWorth; //can be used to determine score increase on collision or life loss on player collision
	float siz; //Nikos Fragkas, 16/02, Needed for score 
	bool exploded; //prevent double explosion at lower framerates

	Vector3 predictedCollisionPosition; //position of pnode, not position of contact point

	int damageOnColis = 15;		//Damage applied to paintable objects on collision

public:
	Projectile();
	Projectile(Colour col, const Vector4& RGBA, Vector3 pos, Vector3 Velocity, float size, float inverseMass, PhysNodeType type, int projectileWorth, const std::string& name = ""); //spherical projectile
	Projectile(Colour col, const Vector4& RGBA, Vector3 pos, Vector3 Velocity, Vector3 size, float inverseMass, PhysNodeType type, int projectileWorth, const std::string& name = ""); //cuboid projectile
	inline float GetSize() { return siz; }
	inline Colour GetColour() { return colour; }

	Vector3 GetPredictedCollisionPosition() {
		return predictedCollisionPosition;
	}
	void SetPredictedCollisionPosition(Vector3 pcp) {
		predictedCollisionPosition = pcp;
	}

	inline int GetDamageOnColision() { return damageOnColis; }
	inline void SetDamageOnColision(int x) { damageOnColis = x; }

	void predictCollisionPosition(Vector3 pos, Vector3 vel);
	
	virtual ~Projectile();

	void Explode(); //paint spray comes out of it

	bool ProjectileCallbackFunction(PhysicsNode* self, PhysicsNode* collidingObject);
	virtual void OnDetachedFromScene();
};