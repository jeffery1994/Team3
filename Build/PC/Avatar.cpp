//					,.ood888888888888boo.,
//              .od888P^""            ""^Y888bo.
//          .od8P''   ..oood88888888booo.    ``Y8bo.
//       .odP'"  .ood8888888888888888888888boo.  "`Ybo.
//     .d8'   od8'd888888888f`8888't888888888b`8bo   `Yb.
//    d8'  od8^   8888888888[  `'  ]8888888888   ^8bo  `8b
//  .8P  d88'     8888888888P      Y8888888888     `88b  Y8.
// d8' .d8'       `Y88888888'      `88888888P'       `8b. `8b
//.8P .88P            """"            """"            Y88. Y8.
//88  888                 Nick Bedford                 888  88
//88  888                 Avatar Class                 888  88
//88  888.        ..       13/02/2018       ..        .888  88
//`8b `88b,     d8888b.od8bo.      .od8bo.d8888b     ,d88' d8'
// Y8. `Y88.    8888888888888b    d8888888888888    .88P' .8P
//  `8b  Y88b.  `88888888888888  88888888888888'  .d88P  d8'
//    Y8.  ^Y88bod8888888888888..8888888888888bod88P^  .8P
//     `Y8.   ^Y888888888888888LS888888888888888P^   .8P'
//       `^Yb.,  `^^Y8888888888888888888888P^^'  ,.dP^'
//          `^Y8b..   ``^^^Y88888888P^^^'    ..d8P^'
//              `^Y888bo.,            ,.od888P^'
//                   "`^^Y888888888888P^^'"         

#include "Avatar.h"
#include <ncltech\SphereCollisionShape.h>
#include <string.h>
#include "GameInput.h"

Avatar::Avatar()
{
	life = maxLife;

	playerGameObject = CommonUtils::BuildSphereObject("Player",
		Vector3(0.0f, 1.0f, 0.0f),
		1.0f,									//Radius
		true,									//Has Physics Object
		1.0f,
		true,									//Has Collision Shape
		false,									//Dragable by the user
		PLAYER,
		Vector4(0.5, 0.5, 0.5, 1.0));	//Color
	canJump = true;
	canShoot = true;

	playerGameObject->Physics()->SetElasticity(0);

	playerId = 0;
	shootCooldown = 0.0f;

	standardSpeed = 5.0f;
	speed = standardSpeed;
	boostedSpeed = standardSpeed * 20;

	maxForce = 30;

	minLife = 10;
	maxLife = 100;
	life = maxLife;

	standardJumpImpulse = 10.0f;
	jumpImpulse = standardJumpImpulse;
	boostedJumpImpulse = standardJumpImpulse * 2;
	shooting = false;
	weapon = NUM_OF_WEAPONS;
	boostactiveTime = 15.0f;
}

Avatar::~Avatar()
{
}

Avatar::Avatar(Vector3 pos, Colour c, uint id, float s)
{
	col = c;
	size = s;

	speed = 5;
	maxForce = 30;

	minLife = 10;
	maxLife = 100;
	life = maxLife;

	jumpImpulse = 10.0f;
	boostactiveTime = 15.0f;
	shootCooldown = 0.0f;

	canJump = true;
	canShoot = true;
	shooting = false;

	switch (c)
	{
	case START_COLOUR:
	{
		colour = DEFAULT_COLOUR;
	}
	case GREEN:
	{
		colour = GREEN_COLOUR;
	}
	break;
	case BLUE:
	{
		colour = BLUE_COLOUR;
	}
	break;
	case RED:
	{
		colour = RED_COLOUR;
	}
	break;
	case PINK
		:
	{
		colour = PINK_COLOUR;
	}
	break;
	default:
	{
		colour = DEFAULT_COLOUR;
	}
	break;
	}

	playerGameObject = CommonUtils::BuildSphereObject("Player",
		Vector3(0.0f, 1.0f*s, 0.0f) + pos,
		1.0f * size,							//Radius
		true,									//Has Physics Object
		1.0f,
		true,									//Has Collision Shape
		false,									//Dragable by the user
		PLAYER,
		colour);								//Colour

	playerGameObject->Physics()->SetElasticity(0);

	playerGameObject->Physics()->SetOnCollisionCallback(
		std::bind(&Avatar::PlayerCallbackFunction,
			this,							//Any non-placeholder param will be passed into the function each time it is called
			std::placeholders::_1,			//The placeholders correlate to the expected parameters being passed to the callback
			std::placeholders::_2
		)
	);

	weapon = NUM_OF_WEAPONS;
	playerId = id;

}

bool Avatar::PlayerCallbackFunction(PhysicsNode* self, PhysicsNode* collidingObject) {

	if (collidingObject->GetType() != PICKUP)
	{
		canJump = true;
		inAir = false;
	}

	return true;
}


void Avatar::ChangeSize(float newSize)
{
	playerGameObject->Render()->GetChild()->SetBoundingRadius(newSize);
	playerGameObject->Render()->SetBoundingRadius(newSize);
	playerGameObject->Physics()->SetBoundingRadius(newSize);
	((SphereCollisionShape*)playerGameObject->Physics()->GetCollisionShape())->SetRadius(newSize);

	playerGameObject->Render()->GetChild()->SetTransform(Matrix4::Scale(Vector3(newSize, newSize, newSize)));
}

// Updates everything on player
void Avatar::OnAvatarUpdate(float dt) {

	shooting = false;
	
	Input(dt);

	ManageWeapons();
	
	UpdatePickUp(dt);

	if (life > minLife)
	{
		
		life -= dt * (float)min((playerGameObject->Physics()->GetLinearVelocity().LengthSQ()) / 2500.0f, 2.0f);

		if (life < minLife)
		{
			life = minLife;
		}
	}

	curSize = size * (life / 100);

	ChangeSize(curSize);

}


void Avatar::PickUpBuffActivated(PickupType pickType) {

	switch (pickType)
	{
	case SPEED_BOOST:
		speed = boostedSpeed;
		speedBoost = true;
		speedTimer = boostactiveTime;
		break;
	case JUMP_BOOST:
		jumpImpulse = boostedJumpImpulse;
		jumpBoost = true;
		jumpBoostTimer = boostactiveTime;
		break;
	case WEAPON: {

	}
				 break;
	default:
		break;
	}


}

void Avatar::UpdatePickUp(float dt)
{
	if (speedBoost)
	{
		speedTimer -= dt;

		if (speedTimer < 0)
		{
			speed = standardSpeed;
			speedBoost = false;
		}
	}

	if (jumpBoost)
	{
		jumpBoostTimer -= dt;

		if (jumpBoostTimer < 0)
		{
			jumpImpulse = standardSpeed;
			jumpBoost = false;
		}
	}

	if (!canShoot) {
		shootCooldown -= dt;
		if (shootCooldown <= 0) {
			canShoot = true;
		}
	}
}

void Avatar::Spray()
{
	/*int randPitch;
	int randYaw;
	Vector3 direction;*/
	
	//Projectile** spray = new Projectile*[15];



	//for (int i = 0; i < 15; i++)
	//{

	//	randPitch = rand() % 180 + -90;
	//	randYaw = rand() % 360;

	//	direction = Matrix3::Rotation(randPitch, Vector3(1, 0, 0)) * Matrix3::Rotation(randYaw, Vector3(0, 1, 0)) * Vector3(0, 0, -1) * 3;

	//  spray[i] = new Projectile(position, velocity, size, enum Colour, Vector4 RGBA);
	
	//  SceneManager::Instance()->GetCurrentScene()->AddGameObject(spray[i]);
	//}
}

void Avatar::ShootRocket()
{
	/*

	Vector3 direction = playerVelocityDirection;

	Rocket* projectile =  new Rocket(position, direction, size, enum Colour, Vector4 RGBA);;

	SceneManager::Instance()->GetCurrentScene()->AddGameObject(projectile);
	*/
}

void Avatar::ShootProjectile()
{
	/*
	
	Vector3 direction = playerVelocityDirection;

	Projectile* projectile =  new Projectile(position, direction, size, enum Colour, Vector4 RGBA);;
	
	SceneManager::Instance()->GetCurrentScene()->AddGameObject(projectile);
	*/
}

void Avatar::ManageWeapons() 
{

	if (shooting)
	{
		switch (weapon)
		{
			case PAINT_SPRAY:
				
				Spray();
				shootCooldown = 3.0f;
				canShoot = false;
				break;

			case PAINT_PISTOL:
				
				ShootProjectile();
				shootCooldown = 0.5f;
				break;

			case AUTO_PAINT_LAUNCHER:
				
				ShootProjectile();
				shootCooldown = 0.15f;
				canShoot = false;
				break;

			case PAINT_ROCKET:
				
				ShootRocket();
				shootCooldown = 0.15f;
				canShoot = false;
				
				break;

			default:
				break;
		}
	}
}