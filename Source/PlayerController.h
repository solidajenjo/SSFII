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
	PlayerController() {}; //TODO: Virtual destructor 

	virtual bool Forward(bool flipped) const;
	virtual bool Backward(bool flipped) const;
	virtual bool Down() const;
	virtual bool Up() const;
	virtual bool L_Punch();
	virtual bool M_Punch();
	virtual bool H_Punch();
	virtual bool L_Kick();  
	virtual bool M_Kick(); 
	virtual bool H_Kick(); 

private:

	PlayerControllerType type;
};


#endif