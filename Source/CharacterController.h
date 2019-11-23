#ifndef __CHARACTERCONTROLLER_H_
#define __CHARACTERCONTROLLER_H_

#include "Globals.h"
#include "ExternalLibraries/MathGeoLib/include/Math/float3.h"
#include <queue>
#include <bitset>

class AnimationSheet;
class Animation;
class PlayerController;
class Fx;

class CharacterController
{
public:
	
	enum class CharacterStates
	{
		IDLE,
		WALK_FORWARD,
		WALK_BACKWARDS,
		ATTACK,
		BLOCK,
		CROUCH,
		CROUCH_ATTACK,
		CROUCH_BLOCK,
		FORWARD_ATTACK,
		BACKWARDS_ATTACK,
		JUMP,
		JUMP_ATTACK,
		JUMP_FORWARD,
		JUMP_FORWARD_ATTACK,
		JUMP_BACKWARDS,
		JUMP_BACKWARDS_ATTACK,
		FACE_HIT,
		BODY_HIT,
		KNOCK_DOWN,
		KO,
		WIN,
		SPECIAL_1,
		SPECIAL_2
	};

	CharacterController(AnimationSheet *animationSheet, float3 pos);

	void Update();

	//members

	CharacterStates state = CharacterStates::IDLE;
	AnimationSheet* animationSheet = nullptr; //TODO:Clean
	Animation* attackAnimation = nullptr;
	Animation* currentAnimation = nullptr;
	PlayerController* controller = nullptr;
	CharacterController* other = nullptr;

	bool loopEnded = false;

	float3 pos = float3::zero;
	float speed = 3.5f; //TODO:Add deltatime
	float verticalSpeed = 0.f;
	float jumpSpeed = 28.f;
	float jumpMovementMultiplier = 1.9f;
	float hitMultiplier = .5f;
	float gravity = 1.2f;
	float idleY = 0.f;
	float direction = 1.f;
	bool isGrounded = true;
	bool isAttacking = false;
	float landingY = 0.f;
	bool flip = false;
	int lifeAmount = 1000;
	int life = lifeAmount;
	unsigned damage = 10u;
	unsigned damageMultiplier = 1u;

	unsigned lastDamage = 0u; //Tells the neural network the damage taken to sum fitness to the rival
	unsigned damageTaken = 0u; //Tells the neural network the damage taken to decrease fitness
	unsigned blocks = 0u;
	int landingWaitTimer = 0u;
	int lastTicks = 0u; //TODO: Do a global delta time
	int landingWaitAmount = 2500u;

	//FXs
	std::queue<Fx*> fxQueue;
	Fx* landingFx = nullptr;

	//Neural Network inputs

	std::string neuralNetworkInput;
	float3 color = float3::one;

private:

	void CheckCrouch();
	void CheckWalk();
	void CheckJump();
	void CheckGroundAttack();
	void CheckAirAttack();
	void CheckLanding();
	void CheckCollision();
	void CheckInsideScreen();	
	void PrepareNeuralNetworkInput();
};


#endif