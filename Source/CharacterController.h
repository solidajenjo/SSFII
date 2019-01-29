#ifndef __CHARACTERCONTROLLER_H_
#define __CHARACTERCONTROLLER_H_

#include "Globals.h"


class Animation;
class AnimationSheet;

class CharacterController
{
public:
	
	enum class CharacterStates
	{
		IDLE,
		WALK_FORWARD,
		WALK_BACKWARDS,
		ATTACK,
		CROUCH,
		CROUCH_ATTACK,
		FORWARD_ATTACK,
		BACKWARDS_ATTACK,
		JUMP,
		JUMP_ATTACK,
		JUMP_FORWARD,
		JUMP_FORWARD_ATTACK,
		JUMP_BACKWARDS,
		JUMP_BACKWARDS_ATTACK,
		KNOCK_DOWN,
		KO		
	};

	
};


#endif