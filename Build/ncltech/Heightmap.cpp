#include "Heightmap.h"
#include "HeightmapCollisionShape.h"

Heightmap::Heightmap(
	const std::string& name,
	const Vector3& pos,
	const Vector3& halfdims,
	float s,
	bool physics_enabled,
	bool collidable,
	const Vector4& color)
{

	RenderNode* rnode = new RenderNode();

	width = halfdims.x / s;
	height = halfdims.z / s;
	m = Mesh::GenerateMesh(width,height,s);
	RenderNode* dummy = new RenderNode(m, Vector4(0.8f,0.8f,0.8f, 1));
	dummy->SetTransform(Matrix4::Scale(Vector3(width,5,height)));

	dummy->SetMaterial(GraphicsPipeline::Instance()->GetAllMaterials()[MATERIALTYPE::Ground]);;

	rnode->AddChild(dummy);
	//dummy->SetCulling(false); 

	rnode->SetTransform(Matrix4::Translation(Vector3(-halfdims.x, 0, -halfdims.y)));
	rnode->SetBoundingRadius(width*4);

	PhysicsNode* pnode = NULL;
	if (physics_enabled)
	{
		pnode = new PhysicsNode();
		pnode->SetPosition(pos);
		pnode->SetInverseMass(0.0f);
		pnode->SetBoundingRadius(width);
		pnode->SetType(BIG_NODE);
		CollisionShape* pColshape = new HeightMapCollisionShape(10, 10,s);
		pnode->SetCollisionShape(pColshape);
		pnode->SetInverseInertia(pColshape->BuildInverseInertia(0.0f));

		this->friendlyName = "Ground";
		this->renderNode = rnode;
		this->physicsNode = pnode;
		RegisterPhysicsToRenderTransformCallback();
		SetPhysics(pnode);
		pnode->SetName("Ground");

		Physics()->SetElasticity(0);
		Physics()->SetOnUpdateCallback(
			std::bind(
				&Heightmap::UpdateMesh,
				this,
				std::placeholders::_1)
		);
	}
}

void Heightmap::UpdateMesh(const Matrix4 &matrix) {

}