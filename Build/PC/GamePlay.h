#pragma once


enum Colour
{
	DEFAULT, RED, GREEN, BLUE, PINK
};

//Name: Nick Bedford
//Date: 06/02/2018
//Added PickupType
enum PickupType 
{
	SPEED_BOOST, JUMP_BOOST, WEAPON 
};

//Name: Nick Bedford
//Date: 07/02/2018
//Added WeaponType
enum WeaponType
{
	PAINT_SPRAY, PAINT_PISTOL, AUTO_PAINT_LAUNCHER, PAINT_ROCKET, NUM_OF_WEAPONS
};

//Game music should always be the last music. So if you add more music, put it before Game music
enum Audio {
	MENU_MUSIC, GAME_MUSIC, JUMP_SOUND
};