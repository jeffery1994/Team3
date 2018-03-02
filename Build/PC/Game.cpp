#include "Game.h"
#include <ncltech\SceneManager.h>
void Game::Update(float dt)
{ 
	if (user)
	{
		user->UpdateUser(dt);
		
	} 
	if (gameRunning) {
		time += dt;
	}
	
	if (time > 30.0f) {
		DetermineWinner();
		//StopGame();
		time = 0.0f;
	}
}

void Game::ResetGame()
{
	for (uint i = 0; i < Game::Instance()->GetPlayerNumber(); ++i)
	{
		SceneManager::Instance()->GetCurrentScene()->RemoveGameObject(avatars[i]);
		avatars[i] = nullptr;
	}
	delete user;
	user = nullptr;
	enet_deinitialize();
	gameRunning = false;
	time = 0.0f;
}

void Game::DetermineWinner() {
	int currentWinner = 0;
	float currentWinnerScore = 0;

	for (int i = 0; i < 4; i++) {
		if (teamScores[i] > currentWinnerScore) {
			currentWinner = i;
			currentWinnerScore = teamScores[i];
		}
	}
	if (currentWinner == RED) {
		cout << "Red Wins!" << endl;
	}
	else if (currentWinner == BLUE) {
		cout << "Blue Wins!" << endl;
	}
	else if (currentWinner == GREEN) {
		cout << "Green Wins!" << endl;
	}
	else if (currentWinner == PINK) {
		cout << "Pink Wins!" << endl;
	}
}