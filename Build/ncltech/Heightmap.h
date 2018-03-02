#pragma once

#include "GameObject.h"

class Heightmap : public GameObject
{
public:
	Heightmap(
		const std::string& name,
		const Vector3& pos,
		const Vector3& halfdims,
		float separation = 1,
		bool physics_enabled = true,
		bool collidable = true,
		const Vector4& color = Vector4(0.5f, 0.5f, 0.5f, 1.0f)
	);

	void UpdateMesh(const Matrix4 &matrix);

	~Heightmap() {};

protected:
	int width;
	int height;
	Mesh * m;
};