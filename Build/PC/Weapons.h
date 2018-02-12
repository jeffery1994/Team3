//Nikos Fragkas
//Date 09/02/2018
//Impliment Weapons

#pragma once
#include <ncltech\GameObject.h>
#include <ncltech\SceneManager.h> 

namespace Weapons
{
	GameObject* BuildPistol(const Vector4& color, float size, Vector3 pos);

	GameObject* BuildRocket(const Vector4& color, float size, Vector3 pos);

	GameObject* BuildPaintSpray(const Vector4& colour, float size, Vector3 pos);



	GameObject* ShootPistol(Vector3 pos, float size, Vector4 colour);
	GameObject* ShootRocket(Vector3 pos, float size, Vector4 colour);
	void ShootPaintSpray(Vector3 pos, float size, Vector4 colour);


}