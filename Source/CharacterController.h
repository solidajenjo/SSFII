#ifndef __CHARACTERCONTROLLER_H_
#define __CHARACTERCONTROLLER_H_

#include "Globals.h"
#include "ExternalLibraries/MathGeoLib/include/Math/float3.h"

class AnimationSheet;
class Animation;
class PlayerController;

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

	CharacterController(AnimationSheet *animationSheet, float3 pos) : animationSheet(animationSheet), pos(pos), idleY(pos.y), landingY(pos.y + 80.f) {};

	void Update();

	//members

	CharacterStates state = CharacterStates::IDLE;
	AnimationSheet* animationSheet = nullptr; //TODO:Clean
	Animation* attackAnimation = nullptr;
	PlayerController* controller = nullptr;
	CharacterController* other = nullptr;

	bool loopEnded = false;

	float3 pos = float3::zero;
	int direction = 1;
	float speed = 2.5f; //TODO:Add deltatime
	float verticalSpeed = 0.f;
	float jumpSpeed = 12.f;
	float gravity = .4f;
	float idleY = 0.f;
	float landingY = 0.f;
	bool flip = false;
	
private:

	void CheckCrouch();
	void CheckWalk();
	void CheckJump();
	void CheckGroundAttack();
	void CheckAirAttack();
};


#endif