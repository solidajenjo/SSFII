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

	bool Forward(bool flipped) const;
	bool Backward(bool flipped) const;
	bool Down() const;
	bool Up() const;
	bool L_Punch() const;
	bool M_Punch() const;
	bool H_Punch() const;
	bool L_Kick() const; 
	bool M_Kick() const;
	bool H_Kick() const;

private:

	PlayerControllerType type;
};


#endif