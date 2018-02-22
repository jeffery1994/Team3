#pragma once
#include <enet\enet.h>
#include <nclgl\GameTimer.h>
#include <nclgl\Vector3.h>
#include <nclgl\common.h>
#include <ncltech\NetworkBase.h>
#include "NetworkCommon.h"
#include <ncltech\SceneManager.h>

class User
{
protected: 
	uint userID = 0;
	string Username[4] = { "Player1","Player2","Player3","Player4" }; //The client's name.
	string ip;
	int mapID = 3;
	bool destroy = false;
public:
	User();
	~User();

	//--------------------------------------------------------------------------------------------//
	// Sending
	//--------------------------------------------------------------------------------------------//
	virtual void SendVector3(uint ID, PacketType type, Vector3 vec) = 0;
	virtual void SendWeaponFire(uint ID, WeaponType type, Vector3 pos, Vector3 dir) = 0;

	//--------------------------------------------------------------------------------------------//
	// Receiving
	//--------------------------------------------------------------------------------------------//

	PlayerVector ReceiveVector(string data);
	PlayerFloat ReceiveSizes(string data);

	void ReceiveWeapon(string data);


	//--------------------------------------------------------------------------------------------//
	// Utility
	//--------------------------------------------------------------------------------------------//
	string GetPacketData(const ENetEvent & evnt);
	
	inline uint GetUserID() { return userID; }
	inline string GetIP() { return ip; }

	inline string GetPlayerName(uint ID) { return Username[ID]; }
	inline void SetPlayerName(uint ID,string name) { Username[ID] = name; }

	inline void SetIP(string _ip) { ip = _ip; }

	virtual void UpdateUser(float dt) = 0;
	virtual void Disconnect() = 0;

	virtual void StartGame(uint mapID = 0);

	ENetPacket* CreatePacket(string data) {
		return enet_packet_create(data.c_str(), sizeof(char) * data.length(), 0);
	}
};

