#include "AnimationSheet.h"
#include "Animation.h"
#include "Game.h"
#include "CharacterController.h"
#include "PlayerController.h"
#include "Animation.h"
#include "AnimationSheet.h"
#include "Input.h"
#include "ExternalLibraries/SDL/include/SDL_timer.h"
#include "Fx.h"

CharacterController::CharacterController(AnimationSheet * animationSheet, float3 pos) : animationSheet(animationSheet), pos(pos), idleY(pos.y), landingY(pos.y + 80.f) 
{
	landingFx = new Fx("landing.fx");
	landingFx->LoadSheet();
	currentAnimation = animationSheet->animations[AnimationSheet::Anims::IDLE];
}

void CharacterController::Update()
{
	assert(animationSheet != nullptr); //null animation sheet on character controller
	assert(other != nullptr); // no enemy set
	
	if (landingWaitTimer > 0)
	{
		landingWaitTimer -= SDL_GetTicks() - lastTicks;
	}

	lastTicks = SDL_GetTicks();

	switch (state)
	{
	case CharacterStates::IDLE:
		isAttacking = false;
		animationSheet->animations[AnimationSheet::Anims::IDLE]->Play(pos, loopEnded, flip);
		currentAnimation = animationSheet->animations[AnimationSheet::Anims::IDLE];
		CheckCrouch();
		CheckWalk();
		CheckJump();
		CheckGroundAttack();
		break;

	case CharacterStates::CROUCH:
		isAttacking = false;
		animationSheet->animations[AnimationSheet::Anims::CROUCH]->Play(pos, loopEnded, flip);		
		if (!controller->Down())
		{
			state = CharacterStates::IDLE;
			animationSheet->animations[AnimationSheet::Anims::IDLE]->Rewind();
		}
		CheckJump();
		CheckGroundAttack();
		if (other->isAttacking && controller->Backward(flip))
		{
			state = CharacterStates::CROUCH_BLOCK;
		}
		break;

	case CharacterStates::WALK_FORWARD:
		isAttacking = false;
		animationSheet->animations[AnimationSheet::Anims::WALK]->Play(pos, loopEnded, flip);
		pos.x += speed * direction;
		if (!controller->Forward(flip))
		{
			state = CharacterStates::IDLE;
			animationSheet->animations[AnimationSheet::Anims::IDLE]->Rewind();
		}
		CheckCrouch();
		CheckJump();
		CheckGroundAttack();		
		break;

	case CharacterStates::WALK_BACKWARDS:
		isAttacking = false;
		animationSheet->animations[AnimationSheet::Anims::WALK]->Play(pos, loopEnded, flip);
		pos.x -= speed * direction;
		if (!controller->Backward(flip))
		{
			state = CharacterStates::IDLE;
			animationSheet->animations[AnimationSheet::Anims::IDLE]->Rewind();
		}
		CheckCrouch();
		CheckJump();
		CheckGroundAttack();
		break;

	case CharacterStates::JUMP: //TODO: Weird artifacts on Ryu's jump animation
		isAttacking = false;
		animationSheet->animations[AnimationSheet::Anims::JUMP]->Play(pos, loopEnded, flip, false);
		pos.y += verticalSpeed;
		verticalSpeed -= gravity;
		CheckLanding();
		CheckAirAttack();
		break;

	case CharacterStates::JUMP_FORWARD:
		isAttacking = false;
		animationSheet->animations[AnimationSheet::Anims::FORWARD_JUMP]->Play(pos, loopEnded, flip, false);
		pos.y += verticalSpeed;
		pos.x += speed * jumpMovementMultiplier * direction;
		verticalSpeed -= gravity;
		CheckLanding();
		CheckAirAttack();
		break;
	
	case CharacterStates::JUMP_BACKWARDS:
		isAttacking = false;
		animationSheet->animations[AnimationSheet::Anims::JUMP]->Play(pos, loopEnded, flip);
		pos.y += verticalSpeed;
		pos.x -= speed * jumpMovementMultiplier * direction;
		verticalSpeed -= gravity;
		CheckLanding();
		CheckAirAttack();		
		break;

	case CharacterStates::ATTACK:
		isAttacking = true;
		attackAnimation->Play(pos, loopEnded, flip);
		currentAnimation = attackAnimation;
		if (loopEnded)
		{
			state = CharacterStates::IDLE;
			animationSheet->animations[AnimationSheet::Anims::IDLE]->Rewind();
		}
		break;

	case CharacterStates::FORWARD_ATTACK:
		isAttacking = true;
		attackAnimation->Play(pos, loopEnded, flip);
		currentAnimation = attackAnimation;
		if (loopEnded)
		{
			state = CharacterStates::IDLE;
			animationSheet->animations[AnimationSheet::Anims::IDLE]->Rewind();
		}
		break;

	case CharacterStates::CROUCH_ATTACK:
		isAttacking = true;
		attackAnimation->Play(pos, loopEnded, flip);
		currentAnimation = attackAnimation;
		if (loopEnded)
		{
			state = CharacterStates::CROUCH;
			animationSheet->animations[AnimationSheet::Anims::IDLE]->Rewind();
		}
		break;

	case CharacterStates::JUMP_FORWARD_ATTACK:
		isAttacking = true;
		if (verticalSpeed < .0f && pos.y < landingY)
		{
			animationSheet->animations[AnimationSheet::Anims::LANDING]->Play(pos, loopEnded, flip);
			currentAnimation = animationSheet->animations[AnimationSheet::Anims::LANDING];
		}
		else
		{
			attackAnimation->Play(pos, loopEnded, flip, false);
			currentAnimation = attackAnimation;
		}
		pos.y += verticalSpeed;
		pos.x += speed * jumpMovementMultiplier * direction;
		verticalSpeed -= gravity;
		
		CheckLanding();

		break;

	case CharacterStates::JUMP_BACKWARDS_ATTACK:
		isAttacking = true;
		if (verticalSpeed < .0f && pos.y < landingY)
		{
			animationSheet->animations[AnimationSheet::Anims::LANDING]->Play(pos, loopEnded, flip);
			currentAnimation = animationSheet->animations[AnimationSheet::Anims::LANDING];
		}
		else
		{
			attackAnimation->Play(pos, loopEnded, flip, false);
			currentAnimation = attackAnimation;
		}
		pos.y += verticalSpeed;
		pos.x -= speed * jumpMovementMultiplier * direction;
		verticalSpeed -= gravity;

		CheckLanding();

		break;

	case CharacterStates::JUMP_ATTACK:
		isAttacking = true;
		if (verticalSpeed < .0f && pos.y < landingY)
		{
			animationSheet->animations[AnimationSheet::Anims::LANDING]->Play(pos, loopEnded, flip);
			currentAnimation = animationSheet->animations[AnimationSheet::Anims::LANDING];
		}
		else
		{
			attackAnimation->Play(pos, loopEnded, flip, false);
			currentAnimation = attackAnimation;
		}
		pos.y += verticalSpeed;
		verticalSpeed -= gravity;
		
		CheckLanding();

		break;

	case CharacterStates::FACE_HIT:
		isAttacking = false;
		currentAnimation = animationSheet->animations[AnimationSheet::Anims::FACE_HIT];
		animationSheet->animations[AnimationSheet::Anims::FACE_HIT]->Play(pos, loopEnded, flip);
		if (loopEnded)
		{
			state = CharacterStates::IDLE;
		}
		pos.x -= speed * hitMultiplier * direction;
		break;

	case CharacterStates::BODY_HIT:
		isAttacking = false;
		currentAnimation = animationSheet->animations[AnimationSheet::Anims::HIT];
		animationSheet->animations[AnimationSheet::Anims::HIT]->Play(pos, loopEnded, flip);
		if (loopEnded)
		{
			state = CharacterStates::IDLE;
		}
		pos.x -= speed * hitMultiplier * direction;
		break;

	case CharacterStates::KNOCK_DOWN:
		isAttacking = false;
		if (!isGrounded)
			pos.x -= speed * direction * 2;		

		if (pos.y > idleY)
		{
			isGrounded = false;
			verticalSpeed -= gravity;
			pos.y += verticalSpeed;			
			animationSheet->animations[AnimationSheet::Anims::KNOCK_DOWN]->Rewind();
		}
		else
		{
			isGrounded = true;
			pos.y = idleY;
		}
		currentAnimation = animationSheet->animations[AnimationSheet::Anims::KNOCK_DOWN];
		animationSheet->animations[AnimationSheet::Anims::KNOCK_DOWN]->Play(pos, loopEnded, flip);
		if (loopEnded)
		{
			state = CharacterStates::IDLE;
		}
		break;
	case CharacterStates::WIN:
		isAttacking = false;
		animationSheet->animations[AnimationSheet::Anims::VICTORY]->Play(pos, loopEnded, flip);
		break;
	case CharacterStates::KO:
		isAttacking = false;
		animationSheet->animations[AnimationSheet::Anims::KO]->Play(pos, loopEnded, flip, false); //TODO: Improve death
		if (pos.y > idleY)
		{
			isGrounded = false;
			verticalSpeed -= gravity;
			pos.y += verticalSpeed;
			animationSheet->animations[AnimationSheet::Anims::KO]->Rewind();
		}
		break;
	case CharacterStates::BLOCK:
		isAttacking = false;
		animationSheet->animations[AnimationSheet::Anims::BLOCKING]->Play(pos, loopEnded, flip, false);
		CheckWalk();
		CheckCrouch();
		CheckWalk();
		CheckJump();
		CheckGroundAttack();
		if (!controller->Backward(flip) || !other->isAttacking)
		{
			state = CharacterStates::IDLE;
			animationSheet->animations[AnimationSheet::Anims::IDLE]->Rewind();
		}
		break;
	case CharacterStates::CROUCH_BLOCK:
		isAttacking = false;
		animationSheet->animations[AnimationSheet::Anims::BLOCKING_CROUCH]->Play(pos, loopEnded, flip, false);
		if (!controller->Backward(flip) || !other->isAttacking)
		{
			state = CharacterStates::CROUCH;			
		}
		break;
	}

	CheckCollision();

	unsigned fxAmount = fxQueue.size();

	while (fxAmount > 0)
	{
		Fx* fx = fxQueue.front();
		fxQueue.pop();
		bool loopEnded;
		fx->animation->Play(float3(fx->position, 0.f), loopEnded, flip, false);
		if (!loopEnded)
			fxQueue.push(fx);
		--fxAmount;
	}

	if (pos.x > other->pos.x && isGrounded)
	{
		flip = true;
		direction = -1.f;
	}
	else if (pos.x < other->pos.x  && isGrounded)
	{
		flip = false;
		direction = 1.f;
	}

	CheckInsideScreen();
	PrepareNeuralNetworkInput();
}

void CharacterController::CheckCrouch()
{
	if (controller->Down())
	{
		state = CharacterStates::CROUCH;		
		animationSheet->animations[AnimationSheet::Anims::CROUCH]->Rewind();
		currentAnimation = animationSheet->animations[AnimationSheet::Anims::CROUCH];
	}
}

void CharacterController::CheckWalk()
{
	if (controller->Backward(flip))
	{
		if (other->isAttacking)
		{
			state = CharacterStates::BLOCK;						
			currentAnimation = animationSheet->animations[AnimationSheet::Anims::BLOCKING];
		}
		else
		{
			state = CharacterStates::WALK_BACKWARDS;
			animationSheet->animations[AnimationSheet::Anims::WALK]->reverse = true;
			animationSheet->animations[AnimationSheet::Anims::WALK]->Rewind();
			currentAnimation = animationSheet->animations[AnimationSheet::Anims::WALK];
		}
	}
	if (controller->Forward(flip))
	{
		state = CharacterStates::WALK_FORWARD;
		animationSheet->animations[AnimationSheet::Anims::WALK]->reverse = false;
		animationSheet->animations[AnimationSheet::Anims::WALK]->Rewind();
		currentAnimation = animationSheet->animations[AnimationSheet::Anims::WALK];
	}
}

void CharacterController::CheckJump()
{
	if (controller->Up())
	{
		verticalSpeed = jumpSpeed;
		switch (state)
		{
		case CharacterStates::IDLE:
			state = CharacterStates::JUMP;
			animationSheet->animations[AnimationSheet::Anims::JUMP]->Rewind();
			currentAnimation = animationSheet->animations[AnimationSheet::Anims::JUMP];
			isGrounded = false;
			break;
		case CharacterStates::WALK_FORWARD:
			state = CharacterStates::JUMP_FORWARD;
			animationSheet->animations[AnimationSheet::Anims::FORWARD_JUMP]->Rewind();
			currentAnimation = animationSheet->animations[AnimationSheet::Anims::FORWARD_JUMP];
			isGrounded = false;
			break;
		case CharacterStates::WALK_BACKWARDS:
			state = CharacterStates::JUMP_BACKWARDS;
			animationSheet->animations[AnimationSheet::Anims::JUMP]->Rewind();
			currentAnimation = animationSheet->animations[AnimationSheet::Anims::JUMP];
			isGrounded = false;
			break;
		}
	}
}

void CharacterController::CheckGroundAttack()
{
	CharacterStates byDistanceState = state;
	/*
		if (distance < X)
			byDistanceState = WALK_FORWARD;
		else
			byDistanceState = IDLE;
	*/
	switch (byDistanceState)
	{
	case(CharacterStates::WALK_BACKWARDS): //remove this when distance enabled
	case (CharacterStates::IDLE):
		if (controller->H_Punch())
		{
			state = CharacterStates::ATTACK;
			attackAnimation = animationSheet->animations[AnimationSheet::Anims::H_PUNCH];
			animationSheet->animations[AnimationSheet::Anims::H_PUNCH]->Rewind();	
			damageMultiplier = 3u;
		}
		if (controller->M_Punch())
		{
			state = CharacterStates::ATTACK;
			attackAnimation = animationSheet->animations[AnimationSheet::Anims::M_PUNCH];
			animationSheet->animations[AnimationSheet::Anims::M_PUNCH]->Rewind();
			damageMultiplier = 2u;
		}
		if (controller->L_Punch())
		{
			state = CharacterStates::ATTACK;
			attackAnimation = animationSheet->animations[AnimationSheet::Anims::L_PUNCH];
			animationSheet->animations[AnimationSheet::Anims::L_PUNCH]->Rewind();
			damageMultiplier = 1u;
		}
		if (controller->H_Kick())
		{
			state = CharacterStates::ATTACK;
			attackAnimation = animationSheet->animations[AnimationSheet::Anims::H_KICK];
			animationSheet->animations[AnimationSheet::Anims::H_KICK]->Rewind();
			damageMultiplier = 3u;
		}
		if (controller->M_Kick())
		{
			state = CharacterStates::ATTACK;
			attackAnimation = animationSheet->animations[AnimationSheet::Anims::M_KICK];
			animationSheet->animations[AnimationSheet::Anims::M_KICK]->Rewind();
			damageMultiplier = 2u;
		}
		if (controller->L_Kick())
		{
			state = CharacterStates::ATTACK;
			attackAnimation = animationSheet->animations[AnimationSheet::Anims::L_KICK];
			animationSheet->animations[AnimationSheet::Anims::L_KICK]->Rewind();
			damageMultiplier = 1u;
		}
		break;

	case CharacterStates::WALK_FORWARD:
		if (controller->H_Punch())
		{
			state = CharacterStates::FORWARD_ATTACK;
			attackAnimation = animationSheet->animations[AnimationSheet::Anims::F_H_PUNCH];
			animationSheet->animations[AnimationSheet::Anims::F_H_PUNCH]->Rewind();
		}
		if (controller->M_Punch())
		{
			state = CharacterStates::FORWARD_ATTACK;
			attackAnimation = animationSheet->animations[AnimationSheet::Anims::F_M_PUNCH];
			animationSheet->animations[AnimationSheet::Anims::F_M_PUNCH]->Rewind();
		}
		if (controller->L_Punch())
		{
			state = CharacterStates::FORWARD_ATTACK;
			attackAnimation = animationSheet->animations[AnimationSheet::Anims::F_L_PUNCH];
			animationSheet->animations[AnimationSheet::Anims::F_L_PUNCH]->Rewind();
		}
		if (controller->H_Kick())
		{
			state = CharacterStates::FORWARD_ATTACK;
			attackAnimation = animationSheet->animations[AnimationSheet::Anims::F_H_KICK];
			animationSheet->animations[AnimationSheet::Anims::F_H_KICK]->Rewind();
		}
		if (controller->M_Kick())
		{
			state = CharacterStates::FORWARD_ATTACK;
			attackAnimation = animationSheet->animations[AnimationSheet::Anims::F_M_KICK];
			animationSheet->animations[AnimationSheet::Anims::F_M_KICK]->Rewind();
		}
		if (controller->L_Kick())
		{
			state = CharacterStates::FORWARD_ATTACK;
			attackAnimation = animationSheet->animations[AnimationSheet::Anims::F_L_KICK];
			animationSheet->animations[AnimationSheet::Anims::F_L_KICK]->Rewind();
		}
		break;

	case CharacterStates::CROUCH:
		if (controller->H_Punch())
		{
			state = CharacterStates::CROUCH_ATTACK;
			attackAnimation = animationSheet->animations[AnimationSheet::Anims::C_H_PUNCH];
			animationSheet->animations[AnimationSheet::Anims::C_H_PUNCH]->Rewind();
		}
		if (controller->M_Punch())
		{
			state = CharacterStates::CROUCH_ATTACK;
			attackAnimation = animationSheet->animations[AnimationSheet::Anims::C_M_PUNCH];
			animationSheet->animations[AnimationSheet::Anims::C_M_PUNCH]->Rewind();
		}
		if (controller->L_Punch())
		{
			state = CharacterStates::CROUCH_ATTACK;
			attackAnimation = animationSheet->animations[AnimationSheet::Anims::C_L_PUNCH];
			animationSheet->animations[AnimationSheet::Anims::C_L_PUNCH]->Rewind();
		}
		if (controller->H_Kick())
		{
			state = CharacterStates::CROUCH_ATTACK;
			attackAnimation = animationSheet->animations[AnimationSheet::Anims::C_H_KICK];
			animationSheet->animations[AnimationSheet::Anims::C_H_KICK]->Rewind();
		}
		if (controller->M_Kick())
		{
			state = CharacterStates::CROUCH_ATTACK;
			attackAnimation = animationSheet->animations[AnimationSheet::Anims::C_M_KICK];
			animationSheet->animations[AnimationSheet::Anims::C_M_KICK]->Rewind();
		}
		if (controller->L_Kick())
		{
			state = CharacterStates::CROUCH_ATTACK;
			attackAnimation = animationSheet->animations[AnimationSheet::Anims::C_L_KICK];
			animationSheet->animations[AnimationSheet::Anims::C_L_KICK]->Rewind();
		}
		break;
	}
}

void CharacterController::CheckAirAttack()
{
	switch (state)
	{
	case CharacterStates::JUMP_FORWARD: //TODO: When flipped this must be reversed
		if (controller->H_Punch())
		{
			state = CharacterStates::JUMP_FORWARD_ATTACK;
			attackAnimation = animationSheet->animations[AnimationSheet::Anims::J_F_H_PUNCH];
			animationSheet->animations[AnimationSheet::Anims::J_F_H_PUNCH]->Rewind();
			damageMultiplier = 3u;
		}
		if (controller->M_Punch())
		{
			state = CharacterStates::JUMP_FORWARD_ATTACK;
			attackAnimation = animationSheet->animations[AnimationSheet::Anims::J_F_M_PUNCH];
			animationSheet->animations[AnimationSheet::Anims::J_F_M_PUNCH]->Rewind();
			damageMultiplier = 2u;
		}
		if (controller->L_Punch())
		{
			state = CharacterStates::JUMP_FORWARD_ATTACK;
			attackAnimation = animationSheet->animations[AnimationSheet::Anims::J_F_L_PUNCH];
			animationSheet->animations[AnimationSheet::Anims::J_F_L_PUNCH]->Rewind();
			damageMultiplier = 1u;
		}
		if (controller->H_Kick())
		{
			state = CharacterStates::JUMP_FORWARD_ATTACK;
			attackAnimation = animationSheet->animations[AnimationSheet::Anims::J_F_H_KICK];
			animationSheet->animations[AnimationSheet::Anims::J_F_H_KICK]->Rewind();
			damageMultiplier = 3u;
		}
		if (controller->M_Kick())
		{
			state = CharacterStates::JUMP_FORWARD_ATTACK;
			attackAnimation = animationSheet->animations[AnimationSheet::Anims::J_F_M_KICK];
			animationSheet->animations[AnimationSheet::Anims::J_F_M_KICK]->Rewind();
			damageMultiplier = 2u;
		}
		if (controller->L_Kick())
		{
			state = CharacterStates::JUMP_FORWARD_ATTACK;
			attackAnimation = animationSheet->animations[AnimationSheet::Anims::J_F_L_KICK];
			animationSheet->animations[AnimationSheet::Anims::J_F_L_KICK]->Rewind();
			damageMultiplier = 1u;
		}
		break;

	case CharacterStates::JUMP:
		if (controller->H_Punch())
		{
			state = CharacterStates::JUMP_ATTACK;
			attackAnimation = animationSheet->animations[AnimationSheet::Anims::J_H_PUNCH];
			animationSheet->animations[AnimationSheet::Anims::J_H_PUNCH]->Rewind();
			damageMultiplier = 3u;
		}
		if (controller->M_Punch())
		{
			state = CharacterStates::JUMP_ATTACK;
			attackAnimation = animationSheet->animations[AnimationSheet::Anims::J_M_PUNCH];
			animationSheet->animations[AnimationSheet::Anims::J_M_PUNCH]->Rewind();
			damageMultiplier = 2u;
		}
		if (controller->L_Punch())
		{
			state = CharacterStates::JUMP_ATTACK;
			attackAnimation = animationSheet->animations[AnimationSheet::Anims::J_L_PUNCH];
			animationSheet->animations[AnimationSheet::Anims::J_L_PUNCH]->Rewind();
			damageMultiplier = 1u;
		}
		if (controller->H_Kick())
		{
			state = CharacterStates::JUMP_ATTACK;
			attackAnimation = animationSheet->animations[AnimationSheet::Anims::J_H_KICK];
			animationSheet->animations[AnimationSheet::Anims::J_H_KICK]->Rewind();
			damageMultiplier = 3u;
		}
		if (controller->M_Kick())
		{
			state = CharacterStates::JUMP_ATTACK;
			attackAnimation = animationSheet->animations[AnimationSheet::Anims::J_M_KICK];
			animationSheet->animations[AnimationSheet::Anims::J_M_KICK]->Rewind();
			damageMultiplier = 2u;
		}
		if (controller->L_Kick())
		{
			state = CharacterStates::JUMP_ATTACK;
			attackAnimation = animationSheet->animations[AnimationSheet::Anims::J_L_KICK];
			animationSheet->animations[AnimationSheet::Anims::J_L_KICK]->Rewind();
			damageMultiplier = 1u;
		}
		break;

	case CharacterStates::JUMP_BACKWARDS:
		if (controller->H_Punch())
		{
			state = CharacterStates::JUMP_BACKWARDS_ATTACK;
			attackAnimation = animationSheet->animations[AnimationSheet::Anims::J_H_PUNCH];
			animationSheet->animations[AnimationSheet::Anims::J_H_PUNCH]->Rewind();
			damageMultiplier = 3u;
		}
		if (controller->M_Punch())
		{
			state = CharacterStates::JUMP_BACKWARDS_ATTACK;
			attackAnimation = animationSheet->animations[AnimationSheet::Anims::J_M_PUNCH];
			animationSheet->animations[AnimationSheet::Anims::J_M_PUNCH]->Rewind();
			damageMultiplier = 2u;
		}
		if (controller->L_Punch())
		{
			state = CharacterStates::JUMP_BACKWARDS_ATTACK;
			attackAnimation = animationSheet->animations[AnimationSheet::Anims::J_L_PUNCH];
			animationSheet->animations[AnimationSheet::Anims::J_L_PUNCH]->Rewind();
			damageMultiplier = 1u;
		}
		if (controller->H_Kick())
		{
			state = CharacterStates::JUMP_BACKWARDS_ATTACK;
			attackAnimation = animationSheet->animations[AnimationSheet::Anims::J_H_KICK];
			animationSheet->animations[AnimationSheet::Anims::J_H_KICK]->Rewind();
			damageMultiplier = 3u;
		}
		if (controller->M_Kick())
		{
			state = CharacterStates::JUMP_BACKWARDS_ATTACK;
			attackAnimation = animationSheet->animations[AnimationSheet::Anims::J_M_KICK];
			animationSheet->animations[AnimationSheet::Anims::J_M_KICK]->Rewind();
			damageMultiplier = 2u;
		}
		if (controller->L_Kick())
		{
			state = CharacterStates::JUMP_BACKWARDS_ATTACK;
			attackAnimation = animationSheet->animations[AnimationSheet::Anims::J_L_KICK];
			animationSheet->animations[AnimationSheet::Anims::J_L_KICK]->Rewind();
			damageMultiplier = 1u;
		}
		break;
	}
}

void CharacterController::CheckLanding()
{
	if (pos.y < idleY)
	{
		state = CharacterStates::IDLE;
		animationSheet->animations[AnimationSheet::Anims::IDLE]->Rewind();
		pos.y = idleY;
		landingFx->position = pos.xy();
		fxQueue.push(landingFx);
		landingFx->animation->Rewind();
		isGrounded = true;
		landingWaitTimer = landingWaitAmount;
	}
}

void CharacterController::CheckCollision()
{
	currentAnimation->UpdateHBoxes(pos.xy(), flip);
	other->currentAnimation->UpdateHBoxes(other->pos.xy(), other->flip);
	currentAnimation->DrawHBoxes();	
	if (currentAnimation->hitBoxes[1].box.Intersects(other->currentAnimation->hitBoxes[1].box))
	{
		if (!flip)
		{
			pos.x -= speed;
		}
		else
		{
			pos.x += speed;
		}
	}

	if (currentAnimation->hitBoxes[2].box.Intersects(other->currentAnimation->hitBoxes[0].box))
	{
		if (other->state != CharacterStates::BLOCK)
		{
			unsigned damageApplied = damage * damageMultiplier;
			other->life -= damageApplied;
			other->lastDamage = damageApplied;
			other->damageTaken += damageApplied;
			if (other->isGrounded)
			{
				other->state = CharacterStates::FACE_HIT;
				other->animationSheet->animations[AnimationSheet::Anims::FACE_HIT]->Rewind();
			}
			else
			{
				other->state = CharacterStates::KNOCK_DOWN;
				other->animationSheet->animations[AnimationSheet::Anims::KNOCK_DOWN]->Rewind();
			}
		}
		else
		{
			++other->blocks;
			other->pos.x -= (other->speed * other->direction) * 5;
		}
		return;
	}

	if (currentAnimation->hitBoxes[2].box.Intersects(other->currentAnimation->hitBoxes[1].box))
	{		
		if (isGrounded)
		{
			if (other->isGrounded && other->state != CharacterStates::CROUCH_BLOCK)
			{
				unsigned damageApplied = damage * damageMultiplier;
				other->life -= damageApplied;
				other->lastDamage += damageApplied;
				other->damageTaken += damageApplied;
				other->state = CharacterStates::BODY_HIT;
				other->animationSheet->animations[AnimationSheet::Anims::HIT]->Rewind();
			}
			else if (!other->isGrounded)
			{
				unsigned damageApplied = damage * damageMultiplier;
				other->life -= damageApplied;
				other->lastDamage += damageApplied;
				other->damageTaken += damageApplied;
				other->state = CharacterStates::KNOCK_DOWN;
				other->animationSheet->animations[AnimationSheet::Anims::KNOCK_DOWN]->Rewind();
			}
			else
			{
				++other->blocks;
				other->pos.x -= (other->speed * other->direction) * 5;
			}
		}
		else
		{
			unsigned damageApplied = damage * damageMultiplier;
			other->life -= damageApplied;
			other->lastDamage += damageApplied;
			other->damageTaken += damageApplied;
			other->state = CharacterStates::KNOCK_DOWN;
			other->animationSheet->animations[AnimationSheet::Anims::KNOCK_DOWN]->Rewind();
		}
		return;
	}

	if (other->life <= 0)
	{
		state = CharacterStates::WIN;
		other->state = CharacterStates::KO;
	}
	
}

void CharacterController::CheckInsideScreen()
{
	if (pos.x < 0)
		pos.x = 0;
	if (pos.x > SCREEN_WIDTH)
		pos.x = SCREEN_WIDTH;
}

void CharacterController::PrepareNeuralNetworkInput()
{
	neuralNetworkInput = std::bitset<25>(pow(2,(int)state)).to_string();
	if (isAttacking)
		neuralNetworkInput[0] = '1';
	else
		neuralNetworkInput[0] = '0';
}