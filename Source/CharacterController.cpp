#include "AnimationSheet.h"
#include "Animation.h"
#include "Game.h"
#include "CharacterController.h"
#include "PlayerController.h"
#include "Animation.h"
#include "AnimationSheet.h"
#include "Input.h"
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
	
	switch (state)
	{
	case CharacterStates::IDLE:
		animationSheet->animations[AnimationSheet::Anims::IDLE]->Play(pos, loopEnded, flip);
		currentAnimation = animationSheet->animations[AnimationSheet::Anims::IDLE];
		CheckCrouch();
		CheckWalk();
		CheckJump();
		CheckGroundAttack();
		break;

	case CharacterStates::CROUCH:
		animationSheet->animations[AnimationSheet::Anims::CROUCH]->Play(pos, loopEnded, flip);		
		if (!controller->Down())
		{
			state = CharacterStates::IDLE;
			animationSheet->animations[AnimationSheet::Anims::IDLE]->Rewind();
		}
		CheckJump();
		CheckGroundAttack();
		break;

	case CharacterStates::WALK_FORWARD:
		animationSheet->animations[AnimationSheet::Anims::WALK]->Play(pos, loopEnded, flip);
		pos.x += speed;
		if (!controller->Right())
		{
			state = CharacterStates::IDLE;
			animationSheet->animations[AnimationSheet::Anims::IDLE]->Rewind();
		}
		CheckCrouch();
		CheckJump();
		CheckGroundAttack();		
		break;

	case CharacterStates::WALK_BACKWARDS:
		animationSheet->animations[AnimationSheet::Anims::WALK]->Play(pos, loopEnded, flip);
		pos.x -= speed;
		if (!controller->Left())
		{
			state = CharacterStates::IDLE;
			animationSheet->animations[AnimationSheet::Anims::IDLE]->Rewind();
		}
		CheckCrouch();
		CheckJump();
		CheckGroundAttack();
		break;

	case CharacterStates::JUMP: //TODO: Weird artifacts on Ryu's jump animation
		animationSheet->animations[AnimationSheet::Anims::JUMP]->Play(pos, loopEnded, flip, false);
		pos.y += verticalSpeed;
		verticalSpeed -= gravity;
		CheckLanding();
		CheckAirAttack();
		break;

	case CharacterStates::JUMP_FORWARD:
		animationSheet->animations[AnimationSheet::Anims::FORWARD_JUMP]->Play(pos, loopEnded, flip, false);
		pos.y += verticalSpeed;
		pos.x += speed * jumpMovementMultiplier;
		verticalSpeed -= gravity;
		CheckLanding();
		CheckAirAttack();
		break;
	
	case CharacterStates::JUMP_BACKWARDS:
		animationSheet->animations[AnimationSheet::Anims::JUMP]->Play(pos, loopEnded, flip);
		pos.y += verticalSpeed;
		pos.x -= speed * jumpMovementMultiplier;
		verticalSpeed -= gravity;
		CheckLanding();
		CheckAirAttack();
		break;

	case CharacterStates::ATTACK:
		attackAnimation->Play(pos, loopEnded, flip);
		currentAnimation = attackAnimation;
		if (loopEnded)
		{
			state = CharacterStates::IDLE;
			animationSheet->animations[AnimationSheet::Anims::IDLE]->Rewind();
		}
		break;

	case CharacterStates::FORWARD_ATTACK:
		attackAnimation->Play(pos, loopEnded, flip);
		currentAnimation = attackAnimation;
		if (loopEnded)
		{
			state = CharacterStates::IDLE;
			animationSheet->animations[AnimationSheet::Anims::IDLE]->Rewind();
		}
		break;

	case CharacterStates::CROUCH_ATTACK:
		attackAnimation->Play(pos, loopEnded, flip);
		currentAnimation = attackAnimation;
		if (loopEnded)
		{
			state = CharacterStates::CROUCH;
			animationSheet->animations[AnimationSheet::Anims::IDLE]->Rewind();
		}
		break;

	case CharacterStates::JUMP_FORWARD_ATTACK:
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
		pos.x += speed * jumpMovementMultiplier;
		verticalSpeed -= gravity;
		
		CheckLanding();

		break;

	case CharacterStates::JUMP_BACKWARDS_ATTACK:
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
		pos.x -= speed * jumpMovementMultiplier;
		verticalSpeed -= gravity;

		CheckLanding();

		break;

	case CharacterStates::JUMP_ATTACK:
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

	flip = false;

	if (other->pos.x < pos.x && isGrounded)
	{
		flip = true;
	}

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
	if (controller->Left())
	{
		state = CharacterStates::WALK_BACKWARDS;
		animationSheet->animations[AnimationSheet::Anims::WALK]->reverse = true;
		animationSheet->animations[AnimationSheet::Anims::WALK]->Rewind();
		currentAnimation = animationSheet->animations[AnimationSheet::Anims::WALK];
	}
	if (controller->Right())
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
		}
		if (controller->M_Punch())
		{
			state = CharacterStates::ATTACK;
			attackAnimation = animationSheet->animations[AnimationSheet::Anims::M_PUNCH];
			animationSheet->animations[AnimationSheet::Anims::M_PUNCH]->Rewind();
		}
		if (controller->L_Punch())
		{
			state = CharacterStates::ATTACK;
			attackAnimation = animationSheet->animations[AnimationSheet::Anims::L_PUNCH];
			animationSheet->animations[AnimationSheet::Anims::L_PUNCH]->Rewind();
		}
		if (controller->H_Kick())
		{
			state = CharacterStates::ATTACK;
			attackAnimation = animationSheet->animations[AnimationSheet::Anims::H_KICK];
			animationSheet->animations[AnimationSheet::Anims::H_KICK]->Rewind();
		}
		if (controller->M_Kick())
		{
			state = CharacterStates::ATTACK;
			attackAnimation = animationSheet->animations[AnimationSheet::Anims::M_KICK];
			animationSheet->animations[AnimationSheet::Anims::M_KICK]->Rewind();
		}
		if (controller->L_Kick())
		{
			state = CharacterStates::ATTACK;
			attackAnimation = animationSheet->animations[AnimationSheet::Anims::L_KICK];
			animationSheet->animations[AnimationSheet::Anims::L_KICK]->Rewind();
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
		}
		if (controller->M_Punch())
		{
			state = CharacterStates::JUMP_FORWARD_ATTACK;
			attackAnimation = animationSheet->animations[AnimationSheet::Anims::J_F_M_PUNCH];
			animationSheet->animations[AnimationSheet::Anims::J_F_M_PUNCH]->Rewind();
		}
		if (controller->L_Punch())
		{
			state = CharacterStates::JUMP_FORWARD_ATTACK;
			attackAnimation = animationSheet->animations[AnimationSheet::Anims::J_F_L_PUNCH];
			animationSheet->animations[AnimationSheet::Anims::J_F_L_PUNCH]->Rewind();
		}
		if (controller->H_Kick())
		{
			state = CharacterStates::JUMP_FORWARD_ATTACK;
			attackAnimation = animationSheet->animations[AnimationSheet::Anims::J_F_H_KICK];
			animationSheet->animations[AnimationSheet::Anims::J_F_H_KICK]->Rewind();
		}
		if (controller->M_Kick())
		{
			state = CharacterStates::JUMP_FORWARD_ATTACK;
			attackAnimation = animationSheet->animations[AnimationSheet::Anims::J_F_M_KICK];
			animationSheet->animations[AnimationSheet::Anims::J_F_M_KICK]->Rewind();
		}
		if (controller->L_Kick())
		{
			state = CharacterStates::JUMP_FORWARD_ATTACK;
			attackAnimation = animationSheet->animations[AnimationSheet::Anims::J_F_L_KICK];
			animationSheet->animations[AnimationSheet::Anims::J_F_L_KICK]->Rewind();
		}
		break;

	case CharacterStates::JUMP:
		if (controller->H_Punch())
		{
			state = CharacterStates::JUMP_ATTACK;
			attackAnimation = animationSheet->animations[AnimationSheet::Anims::J_H_PUNCH];
			animationSheet->animations[AnimationSheet::Anims::J_H_PUNCH]->Rewind();
		}
		if (controller->M_Punch())
		{
			state = CharacterStates::JUMP_ATTACK;
			attackAnimation = animationSheet->animations[AnimationSheet::Anims::J_M_PUNCH];
			animationSheet->animations[AnimationSheet::Anims::J_M_PUNCH]->Rewind();
		}
		if (controller->L_Punch())
		{
			state = CharacterStates::JUMP_ATTACK;
			attackAnimation = animationSheet->animations[AnimationSheet::Anims::J_L_PUNCH];
			animationSheet->animations[AnimationSheet::Anims::J_L_PUNCH]->Rewind();
		}
		if (controller->H_Kick())
		{
			state = CharacterStates::JUMP_ATTACK;
			attackAnimation = animationSheet->animations[AnimationSheet::Anims::J_H_KICK];
			animationSheet->animations[AnimationSheet::Anims::J_H_KICK]->Rewind();
		}
		if (controller->M_Kick())
		{
			state = CharacterStates::JUMP_ATTACK;
			attackAnimation = animationSheet->animations[AnimationSheet::Anims::J_M_KICK];
			animationSheet->animations[AnimationSheet::Anims::J_M_KICK]->Rewind();
		}
		if (controller->L_Kick())
		{
			state = CharacterStates::JUMP_ATTACK;
			attackAnimation = animationSheet->animations[AnimationSheet::Anims::J_L_KICK];
			animationSheet->animations[AnimationSheet::Anims::J_L_KICK]->Rewind();
		}
		break;

	case CharacterStates::JUMP_BACKWARDS:
		if (controller->H_Punch())
		{
			state = CharacterStates::JUMP_BACKWARDS_ATTACK;
			attackAnimation = animationSheet->animations[AnimationSheet::Anims::J_H_PUNCH];
			animationSheet->animations[AnimationSheet::Anims::J_H_PUNCH]->Rewind();
		}
		if (controller->M_Punch())
		{
			state = CharacterStates::JUMP_BACKWARDS_ATTACK;
			attackAnimation = animationSheet->animations[AnimationSheet::Anims::J_M_PUNCH];
			animationSheet->animations[AnimationSheet::Anims::J_M_PUNCH]->Rewind();
		}
		if (controller->L_Punch())
		{
			state = CharacterStates::JUMP_BACKWARDS_ATTACK;
			attackAnimation = animationSheet->animations[AnimationSheet::Anims::J_L_PUNCH];
			animationSheet->animations[AnimationSheet::Anims::J_L_PUNCH]->Rewind();
		}
		if (controller->H_Kick())
		{
			state = CharacterStates::JUMP_BACKWARDS_ATTACK;
			attackAnimation = animationSheet->animations[AnimationSheet::Anims::J_H_KICK];
			animationSheet->animations[AnimationSheet::Anims::J_H_KICK]->Rewind();
		}
		if (controller->M_Kick())
		{
			state = CharacterStates::JUMP_BACKWARDS_ATTACK;
			attackAnimation = animationSheet->animations[AnimationSheet::Anims::J_M_KICK];
			animationSheet->animations[AnimationSheet::Anims::J_M_KICK]->Rewind();
		}
		if (controller->L_Kick())
		{
			state = CharacterStates::JUMP_BACKWARDS_ATTACK;
			attackAnimation = animationSheet->animations[AnimationSheet::Anims::J_L_KICK];
			animationSheet->animations[AnimationSheet::Anims::J_L_KICK]->Rewind();
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
	}
}

void CharacterController::CheckCollision()
{
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
	
}