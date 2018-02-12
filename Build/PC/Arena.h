#pragma once

#include <ncltech\Scene.h>
#include <ncltech\SceneManager.h>
#include <ncltech\PhysicsEngine.h>
#include <ncltech\DistanceConstraint.h>
#include <ncltech\CommonUtils.h>
#include "GamePlay.h"
#include <ncltech\TextureManager.h>
#include "Player.h"


class Arena : public Scene
{
private:

	float m_AccumTime = 0;
	Player* player;

public:
	Arena(const std::string& friendly_name)
		: Scene(friendly_name) {}

	virtual void OnInitializeScene() override;

	virtual void OnUpdateScene(float dt) override;

	virtual void CreateEnvironment();

	~Arena()
	{
		TextureManager::Instance()->RemoveAllTexture();
		delete player;
	}

	void OnCleanupScene() {
		DeleteAllGameObjects();
		TextureManager::Instance()->RemoveAllTexture();
	};

};