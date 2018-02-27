#include "Map.h"

//--------------------------------------------------------------------------------------------//
// Initialization
//--------------------------------------------------------------------------------------------//
void Map::onConnectToScene()
{
	for (uint i = 0; i < Game::Instance()->GetPlayerNumber(); i++) {
		if (Game::Instance()->GetUser())
		{
			Avatar * p = nullptr;
			if (i == Game::Instance()->getUserID())
			{
				p = new ControllableAvatar(spawnPositions[i], Colour(i), i, 1.0f);
			}
			else
			{
				p = new Avatar(spawnPositions[i], Colour(i), i, 1.0f);
			}

			this->AddGameObject(p->GetGameObject());
			Game::Instance()->SetAvatar(i, p);

			GraphicsPipeline::Instance()->AddPlayerRenderNode(Game::Instance()->GetPlayer(i)->GetGameObject()->Render()->GetChild());
		}
	}
}

void Map::OnInitializeScene() {

	GraphicsPipeline::Instance()->SetIsMainMenu(true);
	GraphicsPipeline::Instance()->InitPath(Vector2((float)xDimension, (float)yDimension));

	OnInitializeGUI();

	InitializeScores();

	SetSpawnLocations();

	LoadTextures();

	AddObjects();

	Scene::OnInitializeScene();
}

void Map::OnInitializeGUI()
{
	lifeBar = static_cast<CEGUI::ProgressBar*>(
		GUIsystem::Instance()->createWidget("TaharezLook/ProgressBar",
			Vector4(0.40f, 0.9f, 0.25f, 0.03f),
			Vector4(),
			"lifeBar"
		));

	if (Game::Instance()->GetUser())
	{
		if (Game::Instance()->GetPlayer(Game::Instance()->getUserID()))
			lifeBar->setProgress(Game::Instance()->GetPlayer(Game::Instance()->getUserID())->GetLife() / 100.0f);
	}

	currentPickUp = static_cast<CEGUI::Titlebar*>(
		GUIsystem::Instance()->createWidget("TaharezLook/Titlebar",
			Vector4(0.80f, 0.8f, 0.15f, 0.10f),
			Vector4(),
			"currentPickUp"
		));
	currentPickUp->setText("Current Item: ");
}

void Map::InitializeScores() 
{
	if (Game::Instance()->GetUser())
	{
		if (Game::Instance()->getUserID() == 0)
		{
			score = new Score(xDimension, yDimension, groundScoreAccuracy);
		}
	}
}

void Map::LoadTextures()
{
	if (!TextureManager::Instance()->LoadTexture(TEXTURETYPE::Checker_Board, TEXTUREDIR"checkerboard.tga", GL_REPEAT, GL_NEAREST))
		NCLERROR("Texture not loaded");

	if (!TextureManager::Instance()->LoadCubeMap(TEXTURETYPE::Sky_Box, TEXTUREDIR"SkyBox\\skyright.jpg", TEXTUREDIR"SkyBox\\skyleft.jpg", TEXTUREDIR"SkyBox\\skytop.jpg",
		TEXTUREDIR"SkyBox\\skybottom.jpg", TEXTUREDIR"SkyBox\\skyback.jpg", TEXTUREDIR"SkyBox\\skyfront.jpg"))
		NCLERROR("Texture not loaded");
}

void Map::SetSpawnLocations()
{
	spawnPositions[0] = Vector3(0, 5, 0);
	spawnPositions[1] = Vector3(35, 5, -35);
	spawnPositions[2] = Vector3(-35, 5, 35);
	spawnPositions[3] = Vector3(-35, 5, -35);
}

void Map::OnCleanupScene()
{
	SAFE_DELETE(score);
	//SAFE_DELETE(energyBar);
	DeleteAllGameObjects();
	TextureManager::Instance()->RemoveAllTexture();
	GraphicsPipeline::Instance()->RemoveAllPlayerRenderNode();
};


//--------------------------------------------------------------------------------------------//
// Updating Avatars and Scores
//--------------------------------------------------------------------------------------------//
void Map::OnUpdateScene(float dt)
{
	if(Game::Instance()->getUserID() == 0)
		score->UpdateScores();
	Scene::OnUpdateScene(dt);

	m_AccumTime += dt;

	//player->OnPlayerUpdate(dt);
	for (uint i = 0; i < Game::Instance()->GetPlayerNumber(); i++) {
		if (Game::Instance()->GetPlayer(i))
			Game::Instance()->GetPlayer(i)->OnAvatarUpdate(dt);
	}

	uint drawFlags = PhysicsEngine::Instance()->GetDebugDrawFlags();

	if (Game::Instance()->GetUser())
	{
		if (Game::Instance()->GetPlayer(Game::Instance()->getUserID()))
			lifeBar->setProgress(Game::Instance()->GetCurrentAvatar()->GetLife() / 100.0f);
	}
}
