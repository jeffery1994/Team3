//Michael Davis 16/02/2018
#include "Minion.h"
#include "MinionStates.h"
#include "Game.h" 
#include "Map.h"
#include "Behaviours.h"

Minion::Minion() : GameObject() {
	colour = START_COLOUR;
	RGBA = DEFAULT_COLOUR;
	life = 50;
	size = 0.3f;
}

Minion::Minion(Colour c, Vector4 RGBA, Vector3 position, const string name) : GameObject(name) {
	size = 0.3f;
	RenderNode * rnode = new RenderNode();
	PhysicsNode * pnode = new PhysicsNode();

	RenderNode* dummy = new PlayerRenderNode(CommonMeshes::Sphere(), RGBA);
	dummy->SetTransform(Matrix4::Scale(Vector3(size, size, size)));
	dummy->SetMaterial(GraphicsPipeline::Instance()->GetAllMaterials()[MATERIALTYPE::Forward_Lighting]);
	rnode->AddChild(dummy);

	rnode->GetChild()->SetBaseColor(RGBA);
	rnode->SetTransform(Matrix4::Translation(position));

	pnode->SetBoundingRadius(size);
	rnode->SetBoundingRadius(size);

	pnode->SetPosition(position);
	lastPos = position;
	pnode->SetType(MINION);
	pnode->SetInverseMass(2.0f);

	CollisionShape* pColshape = new SphereCollisionShape(size);
	pnode->SetCollisionShape(pColshape);
	pnode->SetInverseInertia(pColshape->BuildInverseInertia(2.0f));

	pnode->SetOnCollisionCallback(
		std::bind(&Minion::MinionCallbackFunction,
			this,							//Any non-placeholder param will be passed into the function each time it is called
			std::placeholders::_1,			//The placeholders correlate to the expected parameters being passed to the callback
			std::placeholders::_2
		)
	);

	renderNode = rnode;
	physicsNode = pnode;

	RegisterPhysicsToRenderTransformCallback();
	SetPhysics(physicsNode);
	physicsNode->SetName(name);

	dead = false;
	
	minLife = 10;
	maxLife = 50;
	life = maxLife;

	detectionRadius = 5.0f;
	pursueRadius = 7.5f;
	allyHealPursueLimit = 50.0f; 

	colour = c;
	this->RGBA = RGBA;


	GraphicsPipeline::Instance()->AddPlayerRenderNode(dummy);
	((PlayerRenderNode*)Render()->GetChild())->SetIsInAir(false);

	isGrounded = false;
	ChangeState(MinionStateWander::GetInstance());
	physicsNode->SetFriction(0.0f);
	physicsNode->SetElasticity(1.0f);

	ComputeClosestPlayer();
	ComputeNewWanderPosition();
	ComputeClosestCaptureArea();
}

Minion::~Minion() {
	closestCaptureArea = NULL;
	closestPlayer = NULL;
}

void Minion::ChangeState(State<Minion>* newState)
{
	// If new state exists
	if (newState)
	{
		if (currentState) {
			previousState = currentState;
			currentState->Exit(this);
		}
		currentState = newState;
		currentState->Enter(this);
	}

}

void Minion::RevertState()
{
	//If a previous state exists
	if (previousState)
	{
		State<Minion>* temp;
		temp = previousState;
		previousState = currentState;
		currentState->Exit(this);
		currentState = temp;
		currentState->Enter(this);
	}
}

void Minion::Update(float dt)
{
	//float lifeLoss = (Physics()->GetPosition() - lastPos).LengthSQ();
	//life -= lifeLoss / (dt * 10);
	lastPos = Physics()->GetPosition();


	size = 0.3f * (life / 50);

	ChangeSize(size);
	
	if (currentState){
		currentState->Execute(this);
	}

	if (life < minLife) {
		dead = true;
		destroy = true;
	}
	else {
		//////////  AI  ////////////////
		if (minionBlackboard.GetGoToNearestCaptureZone()) {
			if (GetIsGrounded()) {
				if (GetClosestCaptureArea()) {
					Physics()->SetAngularVelocity(Vector3{ 0,0,0 });
					Physics()->SetLinearVelocity(Vector3{ 0,0,0 });
					Physics()->SetAcceleration(Behaviours::Seek(GetClosestCaptureArea()->Physics(), Physics(), GetIsGrounded(), 15, 25));
					SetIsGrounded(false);
				}
			}
			else {
				Physics()->SetAcceleration({ 0, 0, 0 });
			}
		}
		else if (minionBlackboard.GetWander()) {
			if (GetIsGrounded()) {
				Physics()->SetAngularVelocity(Vector3{ 0,0,0 });
				Physics()->SetLinearVelocity(Vector3{ 0,0,0 });
				Physics()->SetAcceleration(Behaviours::Seek(GetWanderPosition(), Physics()->GetPosition(), Physics()->GetLinearVelocity(), GetIsGrounded(), 15, 25));
				SetIsGrounded(false);
			}
			else {
				Physics()->SetAcceleration({ 0, 0, 0 });
			}
		}

		else if (minionBlackboard.GetFleeTarget()) {

		}
		else if (minionBlackboard.GetGoToClosestAlly()) {
			//TODO change this so we have get closestEnemyPlayer and get closestAllyPlayer
			if (GetIsGrounded()) {
				Physics()->SetAngularVelocity(Vector3{ 0,0,0 });
				Physics()->SetLinearVelocity(Vector3{ 0,0,0 });
				Physics()->SetAcceleration(Behaviours::Pursue(GetClosestPlayer()->Physics(), Physics(), GetIsGrounded(), 15, 25));
				SetIsGrounded(false);
			}
			else {
				Physics()->SetAcceleration({ 0, 0, 0 });
			}
		}
		else if (minionBlackboard.GetGoToNearestEnemy()) {
			if (GetIsGrounded()) {
				Physics()->SetAngularVelocity(Vector3{ 0,0,0 });
				Physics()->SetLinearVelocity(Vector3{ 0,0,0 });
				Physics()->SetAcceleration(Behaviours::Pursue(GetClosestPlayer()->Physics(), Physics(), GetIsGrounded(), 15, 25));
				SetIsGrounded(false);
			}
			else {
				Physics()->SetAcceleration({ 0, 0, 0 });
			}
		}
		////////////////////////////////
		//TODO implement AI
		//if no players in range {
		//	state = PATROL;
		//	roam and paint;
		//}
		//if player in range {
		//	state = CHASE;
		//	will try to hit player, either healing if friendly or damaging if enemy
		//}
	}
	wanderTimer += dt;
	closestPlayerTimer += dt;
	closestCaptureAreaTimer += dt;
	if (wanderTimer > 2.0f || WanderPositionInRange()) {
		ComputeNewWanderPosition();
	}
	
	if (closestPlayerTimer > 0.25f) {
		ComputeClosestPlayer();
	}
	if (closestCaptureAreaTimer > 0.75f) {
		ComputeClosestCaptureArea();
	}
}

void Minion::ChangeLife(float l) {
	life += l;
	if (life < minLife) {
		dead = true;
		destroy = true;
	}
	if (life > maxLife) {
		life = maxLife;
	}
}

bool Minion::MinionCallbackFunction(PhysicsNode * self, PhysicsNode * collidingObject) {
	if (collidingObject->GetType() == PLAYER) {
		if (!dead) {
			if (((Avatar*)(collidingObject->GetParent()))->GetColour() != this->colour) {
				((Avatar*)collidingObject->GetParent())->ChangeLife(-(life / 5));
			}
			else ((Avatar*)(collidingObject->GetParent()))->ChangeLife(life / 5);
			dead = true;
			destroy = true;
		}
		return false;
	}
	else if (collidingObject->GetType() == MINION) {
		if (!dead && ((Minion*)(collidingObject->GetParent()))->IsAlive()) {
			if (((Minion*)(collidingObject->GetParent()))->GetColour() != this->colour) {
				float tempLife = life;
				ChangeLife(-((Minion*)collidingObject->GetParent())->GetLife());
				((Minion*)(collidingObject->GetParent()))->ChangeLife(-(tempLife));
				return false;
			}
		}
		return true;
	}
	if (collidingObject->GetType() == BIG_NODE || collidingObject->GetType() == DEFAULT_PHYSICS) {
		isGrounded = true;
		ChangeLife(-1);
		return true;
	}
	if (collidingObject->GetType() == PICKUP) {
		return false;
	}
	return true;
}

void Minion::ChangeSize(float newSize) {
	Render()->GetChild()->SetBoundingRadius(newSize);
	Render()->SetBoundingRadius(newSize);
	Physics()->SetBoundingRadius(newSize);
	((SphereCollisionShape*)Physics()->GetCollisionShape())->SetRadius(newSize);

	Render()->GetChild()->SetTransform(Matrix4::Scale(Vector3(newSize, newSize, newSize)));
}

void Minion::ComputeNewWanderPosition() {
	wanderTimer = 0.0f;
	float randX = rand() % 80 + -40;
	float randZ = rand() % 80 + -40;
	wanderPosition = Vector3{ randX, 2.5f, randZ };
}

void Minion::ComputeClosestPlayer() {
	closestPlayerTimer = 0.0f;
	float minDist = 10000.0f;
	for (int i = 0; i < 4; i++) {
		if (Game::Instance()->GetPlayer(i)) {
			float dist = (this->physicsNode->GetPosition() - Game::Instance()->GetPlayer(i)->Physics()->GetPosition()).Length();
			if (dist < minDist) {
				closestPlayer = Game::Instance()->GetPlayer(i);
				minDist = dist;
			}
		}
	}
}

void Minion::ComputeClosestCaptureArea() {
	closestCaptureAreaTimer = 0.0f;
	float minDist = 10000.0f;
	for (int i = 0; i < ((Map*)SceneManager::Instance()->GetCurrentScene())->GetCaptureAreaVector().size(); i++) {
		if (((Map*)SceneManager::Instance()->GetCurrentScene())->GetCaptureArea(i)->GetColour() != this->colour) {
			float dist = (this->physicsNode->GetPosition() - ((Map*)SceneManager::Instance()->GetCurrentScene())->GetCaptureArea(i)->Physics()->GetPosition()).Length();
			if (dist < minDist) {
				closestCaptureArea = ((Map*)SceneManager::Instance()->GetCurrentScene())->GetCaptureArea(i);
				minDist = dist;
			}
		}
	}
	if (closestCaptureArea) {
		if (closestCaptureArea->GetColour() == this->GetColour()) {
			closestCaptureArea = NULL;
		}
	}
}

bool Minion::WanderPositionInRange() {
	if (abs(wanderPosition.x - physicsNode->GetPosition().x) < 5.f) {
		if (abs(wanderPosition.z - physicsNode->GetPosition().z) < 5.f) {
			return true;
		}
	}
	return false;
}