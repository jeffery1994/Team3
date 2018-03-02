#pragma once

#include "CollisionShape.h"
#include "Hull.h"
#include <nclgl\Vector2.h>

class HeightMapCollisionShape : public CollisionShape
{
public:
	HeightMapCollisionShape(const int w, const int h, const float s);
	HeightMapCollisionShape();
	virtual ~HeightMapCollisionShape();

	// Set Cuboid Dimensions
	void SetHalfWidth(float half_width) { halfDims.x = fabs(half_width); }
	void SetHalfDepth(float half_depth) { halfDims.z = fabs(half_depth); }

	// Get Cuboid Dimensions
	const Vector3& GetHalfDims() const { return halfDims; }
	float GetHalfWidth()	const { return halfDims.x; }
	float GetHalfDepth()	const { return halfDims.z; }

	// Debug Collision Shape
	virtual void DebugDraw() const override;


	// Build Inertia Matrix for rotational mass
	virtual Matrix3 BuildInverseInertia(float invMass) const override;


	// Generic Collision Detection Routines
	//  - Used in CollisionDetectionSAT to identify if two shapes overlap
	virtual void GetCollisionAxes(
		const PhysicsNode* otherObject,
		std::vector<Vector3>& out_axes) const override;

	virtual Vector3 GetClosestPoint(const Vector3& point) const override;

	virtual void GetMinMaxVertexOnAxis(
		const Vector3& axis,
		Vector3& out_min,
		Vector3& out_max) const override;

	virtual void GetIncidentReferencePolygon(
		const Vector3& axis,
		std::list<Vector3>& out_face,
		Vector3& out_normal,
		std::vector<Plane>& out_adjacent_planes) const override;

	//phil 13/02/2018
	virtual float GetRayIntersection(Vector3 origin, Vector3 direction);


protected:
	Vector3 GetPlaneIntersection(Vector3 pNormal, Vector3 pCenter, Vector3 lCenter, Vector3 lNormal);
	//Constructs the static cube hull 
	static void ConstructCubeHull(const int w, const int h);

protected:
	Vector3				 halfDims;
	static Hull			 cubeHull;			//Static cube descriptor, as all cuboid instances will have the same underlying model format ([-1,-1,-1] - [1,1,1] axis aligned cuboid)

};