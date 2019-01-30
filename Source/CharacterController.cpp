#include "AnimationSheet.h"
#include "Animation.h"
#include "Game.h"
#include "CharacterController.h"
#include "Animation.h"
#include "AnimationSheet.h"
#include "Input.h"

void CharacterController::Update()
{
	assert(animationSheet != nullptr); //null animation sheet on character controller
	switch (state)
	{
	case CharacterStates::IDLE:
		animationSheet->animations[AnimationSheet::Anims::IDLE]->Play(pos, loopEnded);
		CheckWalk();
		CheckJump();
		CheckGroundAttack();
		break;

	case CharacterStates::WALK_FORWARD:
		animationSheet->animations[AnimationSheet::Anims::WALK]->Play(pos, loopEnded);
		pos.x += speed;
		if (game->input->GetKey(SDL_SCANCODE_RIGHT) != KEY_REPEAT)
		{
			state = CharacterStates::IDLE;
			animationSheet->animations[AnimationSheet::Anims::IDLE]->Rewind();
		}
		CheckJump();
		CheckGroundAttack();
		break;

	case CharacterStates::WALK_BACKWARDS:
		animationSheet->animations[AnimationSheet::Anims::WALK]->Play(pos, loopEnded);
		pos.x -= speed;
		if (game->input->GetKey(SDL_SCANCODE_LEFT) != KEY_REPEAT)
		{
			state = CharacterStates::IDLE;
			animationSheet->animations[AnimationSheet::Anims::IDLE]->Rewind();
		}
		CheckJump();
		CheckGroundAttack();
		break;

	case CharacterStates::JUMP: //TODO: Weird artifacts on Ryu's jump animation
		animationSheet->animations[AnimationSheet::Anims::JUMP]->Play(pos, loopEnded, false);
		pos.y += verticalSpeed;
		verticalSpeed -= gravity;
		if (pos.y < idleY)
		{
			state = CharacterStates::IDLE;
			pos.y = idleY;
		}
		CheckAirAttack();
		break;

	case CharacterStates::JUMP_FORWARD:
		animationSheet->animations[AnimationSheet::Anims::FORWARD_JUMP]->Play(pos, loopEnded, false);
		pos.y += verticalSpeed;
		pos.x += speed;
		verticalSpeed -= gravity;
		if (pos.y < idleY)
		{
			state = CharacterStates::IDLE;
			pos.y = idleY;
		}
		CheckAirAttack();
		break;
	
	case CharacterStates::JUMP_BACKWARDS:
		animationSheet->animations[AnimationSheet::Anims::JUMP]->Play(pos, loopEnded);
		pos.y += verticalSpeed;
		pos.x -= speed;
		verticalSpeed -= gravity;
		if (pos.y < idleY)
		{
			state = CharacterStates::IDLE;
			pos.y = idleY;
		}
		CheckAirAttack();
		break;

	case CharacterStates::ATTACK:
		attackAnimation->Play(pos, loopEnded);
		if (loopEnded)
		{
			state = CharacterStates::IDLE;
		}
		break;

	case CharacterStates::FORWARD_ATTACK:
		attackAnimation->Play(pos, loopEnded);
		if (loopEnded)
		{
			state = CharacterStates::IDLE;
		}
		break;

	case CharacterStates::JUMP_FORWARD_ATTACK:
		if (verticalSpeed < .0f && pos.y < landingY)
		{
			animationSheet->animations[AnimationSheet::Anims::LANDING]->Play(pos, loopEnded);
		}
		else
			attackAnimation->Play(pos, loopEnded, false);
		pos.y += verticalSpeed;
		pos.x += speed;
		verticalSpeed -= gravity;
		
		if (pos.y < idleY)
		{
			state = CharacterStates::IDLE;
			pos.y = idleY;
		}
		break;

	case CharacterStates::JUMP_BACKWARDS_ATTACK:
		if (verticalSpeed < .0f && pos.y < landingY)
		{
			animationSheet->animations[AnimationSheet::Anims::LANDING]->Play(pos, loopEnded);
		}
		else
			attackAnimation->Play(pos, loopEnded, false);
		pos.y += verticalSpeed;
		pos.x -= speed;
		verticalSpeed -= gravity;

		if (pos.y < idleY)
		{
			state = CharacterStates::IDLE;
			pos.y = idleY;
		}
		break;

	case CharacterStates::JUMP_ATTACK:
		if (verticalSpeed < .0f && pos.y < landingY)
		{
			animationSheet->animations[AnimationSheet::Anims::LANDING]->Play(pos, loopEnded);
		}
		else
			attackAnimation->Play(pos, loopEnded, false);
		pos.y += verticalSpeed;
		verticalSpeed -= gravity;
		
		if (pos.y < idleY)
		{
			state = CharacterStates::IDLE;
			pos.y = idleY;
		}
		break;

	}


}

void CharacterController::CheckWalk()
{
	if (game->input->GetKey(SDL_SCANCODE_LEFT) == KEY_DOWN || game->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
	{
		state = CharacterStates::WALK_BACKWARDS;
		animationSheet->animations[AnimationSheet::Anims::WALK]->reverse = true;
		animationSheet->animations[AnimationSheet::Anims::WALK]->Rewind();
	}
	if (game->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_DOWN || game->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
	{
		state = CharacterStates::WALK_FORWARD;
		animationSheet->animations[AnimationSheet::Anims::WALK]->reverse = false;
		animationSheet->animations[AnimationSheet::Anims::WALK]->Rewind();
	}
}

void CharacterController::CheckJump()
{
	if (game->input->GetKey(SDL_SCANCODE_UP) == KEY_DOWN || game->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
	{
		verticalSpeed = jumpSpeed;
		switch (state)
		{
		case CharacterStates::IDLE:
			if (game->input->GetKey(SDL_SCANCODE_UP) == KEY_DOWN || game->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
				state = CharacterStates::JUMP;
			animationSheet->animations[AnimationSheet::Anims::JUMP]->Rewind();
			break;
		case CharacterStates::WALK_FORWARD:
			if (game->input->GetKey(SDL_SCANCODE_UP) == KEY_DOWN || game->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
				state = CharacterStates::JUMP_FORWARD;
			animationSheet->animations[AnimationSheet::Anims::FORWARD_JUMP]->Rewind();
			break;
		case CharacterStates::WALK_BACKWARDS:
			if (game->input->GetKey(SDL_SCANCODE_UP) == KEY_DOWN || game->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
				state = CharacterStates::JUMP_BACKWARDS;
			animationSheet->animations[AnimationSheet::Anims::JUMP]->Rewind();
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
		if (game->input->GetKey(SDL_SCANCODE_A) == KEY_DOWN)
		{
			state = CharacterStates::ATTACK;
			attackAnimation = animationSheet->animations[AnimationSheet::Anims::H_PUNCH];
			animationSheet->animations[AnimationSheet::Anims::H_PUNCH]->Rewind();
		}
		if (game->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN)
		{
			state = CharacterStates::ATTACK;
			attackAnimation = animationSheet->animations[AnimationSheet::Anims::M_PUNCH];
			animationSheet->animations[AnimationSheet::Anims::M_PUNCH]->Rewind();
		}
		if (game->input->GetKey(SDL_SCANCODE_D) == KEY_DOWN)
		{
			state = CharacterStates::ATTACK;
			attackAnimation = animationSheet->animations[AnimationSheet::Anims::L_PUNCH];
			animationSheet->animations[AnimationSheet::Anims::L_PUNCH]->Rewind();
		}
		if (game->input->GetKey(SDL_SCANCODE_Z) == KEY_DOWN)
		{
			state = CharacterStates::ATTACK;
			attackAnimation = animationSheet->animations[AnimationSheet::Anims::H_KICK];
			animationSheet->animations[AnimationSheet::Anims::H_KICK]->Rewind();
		}
		if (game->input->GetKey(SDL_SCANCODE_X) == KEY_DOWN)
		{
			state = CharacterStates::ATTACK;
			attackAnimation = animationSheet->animations[AnimationSheet::Anims::M_KICK];
			animationSheet->animations[AnimationSheet::Anims::M_KICK]->Rewind();
		}
		if (game->input->GetKey(SDL_SCANCODE_C) == KEY_DOWN)
		{
			state = CharacterStates::ATTACK;
			attackAnimation = animationSheet->animations[AnimationSheet::Anims::L_KICK];
			animationSheet->animations[AnimationSheet::Anims::L_KICK]->Rewind();
		}
		break;

	case CharacterStates::WALK_FORWARD:
		if (game->input->GetKey(SDL_SCANCODE_A) == KEY_DOWN)
		{
			state = CharacterStates::FORWARD_ATTACK;
			attackAnimation = animationSheet->animations[AnimationSheet::Anims::F_H_PUNCH];
			animationSheet->animations[AnimationSheet::Anims::F_H_PUNCH]->Rewind();
		}
		if (game->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN)
		{
			state = CharacterStates::FORWARD_ATTACK;
			attackAnimation = animationSheet->animations[AnimationSheet::Anims::F_M_PUNCH];
			animationSheet->animations[AnimationSheet::Anims::F_M_PUNCH]->Rewind();
		}
		if (game->input->GetKey(SDL_SCANCODE_D) == KEY_DOWN)
		{
			state = CharacterStates::FORWARD_ATTACK;
			attackAnimation = animationSheet->animations[AnimationSheet::Anims::F_L_PUNCH];
			animationSheet->animations[AnimationSheet::Anims::F_L_PUNCH]->Rewind();
		}
		if (game->input->GetKey(SDL_SCANCODE_Z) == KEY_DOWN)
		{
			state = CharacterStates::FORWARD_ATTACK;
			attackAnimation = animationSheet->animations[AnimationSheet::Anims::F_H_KICK];
			animationSheet->animations[AnimationSheet::Anims::F_H_KICK]->Rewind();
		}
		if (game->input->GetKey(SDL_SCANCODE_X) == KEY_DOWN)
		{
			state = CharacterStates::FORWARD_ATTACK;
			attackAnimation = animationSheet->animations[AnimationSheet::Anims::F_M_KICK];
			animationSheet->animations[AnimationSheet::Anims::F_M_KICK]->Rewind();
		}
		if (game->input->GetKey(SDL_SCANCODE_C) == KEY_DOWN)
		{
			state = CharacterStates::FORWARD_ATTACK;
			attackAnimation = animationSheet->animations[AnimationSheet::Anims::F_L_KICK];
			animationSheet->animations[AnimationSheet::Anims::F_L_KICK]->Rewind();
		}
		break;
	}
}

void CharacterController::CheckAirAttack()
{
	switch (state)
	{
	case CharacterStates::JUMP_FORWARD:
		if (game->input->GetKey(SDL_SCANCODE_A) == KEY_DOWN)
		{
			state = CharacterStates::JUMP_FORWARD_ATTACK;
			attackAnimation = animationSheet->animations[AnimationSheet::Anims::J_F_H_PUNCH];
			animationSheet->animations[AnimationSheet::Anims::J_F_H_PUNCH]->Rewind();
		}
		if (game->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN)
		{
			state = CharacterStates::JUMP_FORWARD_ATTACK;
			attackAnimation = animationSheet->animations[AnimationSheet::Anims::J_F_M_PUNCH];
			animationSheet->animations[AnimationSheet::Anims::J_F_M_PUNCH]->Rewind();
		}
		if (game->input->GetKey(SDL_SCANCODE_D) == KEY_DOWN)
		{
			state = CharacterStates::JUMP_FORWARD_ATTACK;
			attackAnimation = animationSheet->animations[AnimationSheet::Anims::J_F_L_PUNCH];
			animationSheet->animations[AnimationSheet::Anims::J_F_L_PUNCH]->Rewind();
		}
		if (game->input->GetKey(SDL_SCANCODE_Z) == KEY_DOWN)
		{
			state = CharacterStates::JUMP_FORWARD_ATTACK;
			attackAnimation = animationSheet->animations[AnimationSheet::Anims::J_F_H_KICK];
			animationSheet->animations[AnimationSheet::Anims::J_F_H_KICK]->Rewind();
		}
		if (game->input->GetKey(SDL_SCANCODE_X) == KEY_DOWN)
		{
			state = CharacterStates::JUMP_FORWARD_ATTACK;
			attackAnimation = animationSheet->animations[AnimationSheet::Anims::J_F_M_KICK];
			animationSheet->animations[AnimationSheet::Anims::J_F_M_KICK]->Rewind();
		}
		if (game->input->GetKey(SDL_SCANCODE_C) == KEY_DOWN)
		{
			state = CharacterStates::JUMP_FORWARD_ATTACK;
			attackAnimation = animationSheet->animations[AnimationSheet::Anims::J_F_L_KICK];
			animationSheet->animations[AnimationSheet::Anims::J_F_L_KICK]->Rewind();
		}
		break;

	case CharacterStates::JUMP:
		if (game->input->GetKey(SDL_SCANCODE_A) == KEY_DOWN)
		{
			state = CharacterStates::JUMP_ATTACK;
			attackAnimation = animationSheet->animations[AnimationSheet::Anims::J_H_PUNCH];
			animationSheet->animations[AnimationSheet::Anims::J_H_PUNCH]->Rewind();
		}
		if (game->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN)
		{
			state = CharacterStates::JUMP_ATTACK;
			attackAnimation = animationSheet->animations[AnimationSheet::Anims::J_M_PUNCH];
			animationSheet->animations[AnimationSheet::Anims::J_M_PUNCH]->Rewind();
		}
		if (game->input->GetKey(SDL_SCANCODE_D) == KEY_DOWN)
		{
			state = CharacterStates::JUMP_ATTACK;
			attackAnimation = animationSheet->animations[AnimationSheet::Anims::J_L_PUNCH];
			animationSheet->animations[AnimationSheet::Anims::J_L_PUNCH]->Rewind();
		}
		if (game->input->GetKey(SDL_SCANCODE_Z) == KEY_DOWN)
		{
			state = CharacterStates::JUMP_ATTACK;
			attackAnimation = animationSheet->animations[AnimationSheet::Anims::J_H_KICK];
			animationSheet->animations[AnimationSheet::Anims::J_H_KICK]->Rewind();
		}
		if (game->input->GetKey(SDL_SCANCODE_X) == KEY_DOWN)
		{
			state = CharacterStates::JUMP_ATTACK;
			attackAnimation = animationSheet->animations[AnimationSheet::Anims::J_M_KICK];
			animationSheet->animations[AnimationSheet::Anims::J_M_KICK]->Rewind();
		}
		if (game->input->GetKey(SDL_SCANCODE_C) == KEY_DOWN)
		{
			state = CharacterStates::JUMP_ATTACK;
			attackAnimation = animationSheet->animations[AnimationSheet::Anims::J_L_KICK];
			animationSheet->animations[AnimationSheet::Anims::J_L_KICK]->Rewind();
		}
		break;

	case CharacterStates::JUMP_BACKWARDS:
		if (game->input->GetKey(SDL_SCANCODE_A) == KEY_DOWN)
		{
			state = CharacterStates::JUMP_BACKWARDS_ATTACK;
			attackAnimation = animationSheet->animations[AnimationSheet::Anims::J_H_PUNCH];
			animationSheet->animations[AnimationSheet::Anims::J_H_PUNCH]->Rewind();
		}
		if (game->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN)
		{
			state = CharacterStates::JUMP_BACKWARDS_ATTACK;
			attackAnimation = animationSheet->animations[AnimationSheet::Anims::J_M_PUNCH];
			animationSheet->animations[AnimationSheet::Anims::J_M_PUNCH]->Rewind();
		}
		if (game->input->GetKey(SDL_SCANCODE_D) == KEY_DOWN)
		{
			state = CharacterStates::JUMP_BACKWARDS_ATTACK;
			attackAnimation = animationSheet->animations[AnimationSheet::Anims::J_L_PUNCH];
			animationSheet->animations[AnimationSheet::Anims::J_L_PUNCH]->Rewind();
		}
		if (game->input->GetKey(SDL_SCANCODE_Z) == KEY_DOWN)
		{
			state = CharacterStates::JUMP_BACKWARDS_ATTACK;
			attackAnimation = animationSheet->animations[AnimationSheet::Anims::J_H_KICK];
			animationSheet->animations[AnimationSheet::Anims::J_H_KICK]->Rewind();
		}
		if (game->input->GetKey(SDL_SCANCODE_X) == KEY_DOWN)
		{
			state = CharacterStates::JUMP_BACKWARDS_ATTACK;
			attackAnimation = animationSheet->animations[AnimationSheet::Anims::J_M_KICK];
			animationSheet->animations[AnimationSheet::Anims::J_M_KICK]->Rewind();
		}
		if (game->input->GetKey(SDL_SCANCODE_C) == KEY_DOWN)
		{
			state = CharacterStates::JUMP_BACKWARDS_ATTACK;
			attackAnimation = animationSheet->animations[AnimationSheet::Anims::J_L_KICK];
			animationSheet->animations[AnimationSheet::Anims::J_L_KICK]->Rewind();
		}
		break;
	}
}


