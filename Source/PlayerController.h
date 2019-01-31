#ifndef __PLAYERCONTROLLER_H_
#define __PLAYERCONTROLLER_H_

#include "Globals.h"

enum PlayerControllerType
{
	KEYBOARD,
	JOYSTICK
};

class PlayerController
{
public:

	PlayerController(PlayerControllerType type) : type(type) {}

	bool Left();
	bool Right();
	bool Down();
	bool Up();
	bool L_Punch();
	bool M_Punch();
	bool H_Punch();
	bool L_Kick();
	bool M_Kick();
	bool H_Kick();

private:

	PlayerControllerType type;
};


#endif