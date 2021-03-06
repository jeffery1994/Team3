#include "AudioSystem.h"

//Michael Davis 08/02/2018

//constructor, initialises all variables automatically
AudioSystem::AudioSystem() {
	myfile.open("../AudioFiles/AudioLog.txt");

	result = FMOD::System_Create(&audioSystem);
	myfile << "System created: " << result << std::endl;

	int driverCount = 0;
	result = audioSystem->getNumDrivers(&driverCount);
	myfile << "Got Drivers: " << result << std::endl;
	if (driverCount == 0) {
		//TODO some kind of ui information letting the user know they don't have appropriate drivers
		std::cout << "No Audio Driver Detected!" << std::endl;
	}

	listenerPos = { 0.0f, 0.0f, 0.0f };
	listenerLastPos = { 0.0f, 0.0f, 0.0f };
	listenerForward = { 0.0f, 0.0f, 1.0f };
	listenerUp = { 0.0f, 1.0f, 0.0f };
	listenerVelocity = { 0.0f, 0.0f, 0.0f };

	masterVolume = 1.0f;
	gameSoundsVolume = 1.0f;
	musicVolume = 1.0f;

	result = audioSystem->init(32, FMOD_INIT_3D_RIGHTHANDED, NULL);
	myfile << "Audio System initialised: " << result << std::endl;
	result = audioSystem->set3DSettings(1.0f, 1.0f, 1.0f);
	myfile << "3D settings: " << result << std::endl;

	result = audioSystem->createChannelGroup("GameSoundsGroup", &gameSoundsGroup);
	myfile << "Created GameSounds channel group: " << result << std::endl;
	result = audioSystem->createChannelGroup("MusicGroup", &musicGroup);
	myfile << "Created Music channel group: " << result << std::endl;

	result = audioSystem->getMasterChannelGroup(&masterGroup);
	myfile << "Created Master channel group: " << result << std::endl;
	result = masterGroup->addGroup(gameSoundsGroup);
	myfile << "Released Audio System: " << result << std::endl;
	myfile.close();
}

//destructor
AudioSystem::~AudioSystem() {
	for (int i = 0; i < numSounds; i++) {
		result = sounds[i]->release();
		myfile << "Released sound at index(" << i << "): " << result << std::endl;
	}
	result = audioSystem->close();
	myfile << "Closed audio system: " << result << std::endl;
	result = audioSystem->release();
	myfile << "Released Audio System: " << result << std::endl;
	myfile.close();
}

//adds a sound to the system. For smaller files
void AudioSystem::Create2DSound(int index, const char* pFile) {
	FMOD_RESULT result;
	if (sounds[index] == NULL) {
		result = audioSystem->createSound(pFile, FMOD_2D, 0, &sounds[index]);
	}
	else {
		sounds[index]->release();
		result = audioSystem->createSound(pFile, FMOD_2D, 0, &sounds[index]);
	}
	myfile << "Created 2D sound at index(" << index << "): " << result << std::endl;
}

//3D sounds, attenuation is controlled with minDist and maxDist.
void AudioSystem::Create3DSound(int index, const char * pFile, float minDist, float maxDist) {
	if (sounds[index] == NULL) {
		//TODO look into attenuation a bit more and the 3D sound types
		result = audioSystem->createSound(pFile, FMOD_3D_LINEARROLLOFF, 0, &sounds[index]);
		sounds[index]->set3DMinMaxDistance(minDist, maxDist);
	}
	else {
		sounds[index]->release();
		result = audioSystem->createSound(pFile, FMOD_3D_LINEARROLLOFF, 0, &sounds[index]);
		sounds[index]->set3DMinMaxDistance(minDist, maxDist);
	}
	myfile << "Created 3D sound at index(" << index << "): " << result << std::endl;
}

//makes the sound a stream. For larger files that may take a while to load
void AudioSystem::Create2DStream(int index, const char* pFile) {
	if (sounds[index] == NULL) {
		result = audioSystem->createStream(pFile, FMOD_2D, 0, &sounds[index]);
	}
	else {
		sounds[index]->release();
		result = audioSystem->createStream(pFile, FMOD_2D, 0, &sounds[index]);
	}
	myfile << "Created 2D stream at index(" << index << "): " << result << std::endl;
}

//3D streams. May be a large file that needs to be streamed constantly in 3D space.
void AudioSystem::Create3DStream(int index, const char* pFile, float minDist, float maxDist) {
	if (sounds[index] == NULL) {
		result = audioSystem->createStream(pFile, FMOD_3D_LINEARROLLOFF, 0, &sounds[index]);
		sounds[index]->set3DMinMaxDistance(minDist, maxDist);
	}
	else {
		sounds[index]->release();
		result = audioSystem->createStream(pFile, FMOD_3D_LINEARROLLOFF, 0, &sounds[index]);
		sounds[index]->set3DMinMaxDistance(minDist, maxDist);
	}
	myfile << "Created 3D stream at index(" << index << "): " << result << std::endl;
}

//plays a certain sound, works for both sounds and streams
void AudioSystem::PlayASound(int index, bool loop, Vector3 position, Vector3 velocity) {
	if (!loop)
		sounds[index]->setMode(FMOD_LOOP_OFF);
	else
	{
		sounds[index]->setMode(FMOD_LOOP_NORMAL);
		sounds[index]->setLoopCount(-1);
	}

	FMOD_VECTOR fPosition = toFMODVector(position);
	FMOD_VECTOR fVelocity = toFMODVector(velocity);

	result = audioSystem->playSound(sounds[index], 0, false, &channels[index]);
	if (index <= GAME_MUSIC) {
		channels[index]->setChannelGroup(musicGroup);
		channels[index]->set3DAttributes(&fPosition, &fVelocity);
	}
	else {
		channels[index]->setChannelGroup(gameSoundsGroup);
		channels[index]->set3DAttributes(&fPosition, &fVelocity);
	}
	myfile << "Played sound at index(" << index << "): " << result << std::endl;
}

//memory management, will delete the sound
void AudioSystem::ReleaseSound(int index) {
	result = sounds[index]->release();
	myfile << "Released sound at index(" << index << "): " << result << std::endl;
}

//call each frame to update the audiosystem and pass in camera parameters
void AudioSystem::Update(Vector3 cameraPos, Vector3 cameraForward, Vector3 cameraUp, float dt) {
	listenerLastPos = listenerPos;
	listenerPos = toFMODVector(cameraPos);
	listenerForward = toFMODVector(cameraForward);
	listenerUp = toFMODVector(cameraUp);

	listenerVelocity.x = (listenerPos.x - listenerLastPos.x) * (dt / 1000);
	listenerVelocity.y = (listenerPos.y - listenerLastPos.y) * (dt / 1000);
	listenerVelocity.z = (listenerPos.z - listenerLastPos.z) * (dt / 1000);

	audioSystem->set3DListenerAttributes(0, &listenerPos, &listenerVelocity, &listenerForward, &listenerUp);
	audioSystem->update();
}

//stop a specific sound, not pause
//TODO add rest of debug messages
void AudioSystem::StopSound(int index) {
	result = channels[index]->stop();
	myfile << "Stop sound at index(" << index << "): " << result << std::endl;
}

//stop all sounds, not pause
void AudioSystem::StopAllSounds() {
	result = masterGroup->stop();
	myfile << "Stopped mastergroup sounds: " << result << std::endl;
}

//sets the master volume
void AudioSystem::SetMasterVolume(float f) {
	masterVolume = f;
	result = masterGroup->setVolume(masterVolume);
	myfile << "Set mastergroup volume to " << masterVolume << ": " << result << std::endl;
}

//sets all game sounds volume
void AudioSystem::SetGameSoundsVolume(float f) {
	gameSoundsVolume = f;
	result = gameSoundsGroup->setVolume(gameSoundsVolume);
	myfile << "Set GameSoundsGroup volume to " << gameSoundsVolume << ": " << result << std::endl;
}

//sets all music volume
void AudioSystem::SetMusicVolume(float f) {
	musicVolume = f;
	result = musicGroup->setVolume(musicVolume);
	myfile << "Set musicgroup volume to " << musicVolume << ": " << result << std::endl;
}

//pause all game sounds in the game
void AudioSystem::PauseGameSounds() {
	result = gameSoundsGroup->setPaused(1);
	myfile << "Paused game sounds: " << result << std::endl;
}
//unpause all game sounds in the game
void AudioSystem::UnpauseGameSounds() {
	result = gameSoundsGroup->setPaused(0);
	myfile << "Unpaused game sounds: " << result << std::endl;
}

//pause all music in the game
void AudioSystem::PauseMusic() {
	result = musicGroup->setPaused(1);
	myfile << "Paused music: " << result << std::endl;
}

//unpause all music in the game
void AudioSystem::UnpauseMusic() {
	result = musicGroup->setPaused(0);
	myfile << "Unpaused music: " << result << std::endl;
}

//mutes all sounds, continue playing
void AudioSystem::MuteAllSounds() {
	result = masterGroup->setMute(1);
	myfile << "Mute all sounds: " << result << std::endl;
}

//unmutes all sounds
void AudioSystem::UnmuteAllSounds() {
	result = masterGroup->setMute(0);
	myfile << "Unmute all sounds: " << result << std::endl;
}

//converts vec3 to FMODVec
FMOD_VECTOR toFMODVector(Vector3 v) {
	FMOD_VECTOR fm;
	fm.x = v.x;
	fm.y = v.y;
	fm.z = v.z;
	return fm;
}