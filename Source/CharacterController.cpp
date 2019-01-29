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
		if (game->input->GetKey(SDL_SCANCODE_LEFT) == KEY_DOWN || game->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		{
			state = CharacterStates::WALK_BACKWARDS;
			animationSheet->animations[AnimationSheet::Anims::WALK]->reverse = true;
		}
		if (game->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_DOWN || game->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		{
			state = CharacterStates::WALK_FORWARD;
			animationSheet->animations[AnimationSheet::Anims::WALK]->reverse = false;
		}
		if (game->input->GetKey(SDL_SCANCODE_UP) == KEY_DOWN || game->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		{
			state = CharacterStates::JUMP;
			animationSheet->animations[AnimationSheet::Anims::WALK]->reverse = false;
			animationSheet->animations[AnimationSheet::Anims::WALK]->Rewind();
			verticalSpeed = jumpSpeed;
		}
		break;

	case CharacterStates::WALK_FORWARD:		
		animationSheet->animations[AnimationSheet::Anims::WALK]->Play(pos, loopEnded);
		pos.x += speed;
		if (game->input->GetKey(SDL_SCANCODE_RIGHT) != KEY_REPEAT)
		{
			state = CharacterStates::IDLE;
		}
		break;
	case CharacterStates::WALK_BACKWARDS:
		animationSheet->animations[AnimationSheet::Anims::WALK]->Play(pos, loopEnded);
		pos.x -= speed;
		if (game->input->GetKey(SDL_SCANCODE_LEFT) != KEY_REPEAT)
		{
			state = CharacterStates::IDLE;
		}
		break;
	case CharacterStates::JUMP: //TODO: Weird artifacts on Ryu's jump animation
		animationSheet->animations[AnimationSheet::Anims::JUMP]->Play(pos, loopEnded);
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
