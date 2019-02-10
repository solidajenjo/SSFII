#include "PlayerController.h"
#include "Game.h"
#include "Input.h"

bool PlayerController::Forward(bool flipped) const
{
	if (!flipped)
	{
		switch (type)
		{
		case KEYBOARD:
			return game->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_DOWN || game->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT;
			break;
		case JOYSTICK:
			return game->input->GetJoy(JOY_RIGHT) == KEY_REPEAT;
			break;
		}
	}
	else
	{
		switch (type)
		{
		case KEYBOARD:
			return game->input->GetKey(SDL_SCANCODE_LEFT) == KEY_DOWN || game->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT;
			break;
		case JOYSTICK:
			return game->input->GetJoy(JOY_LEFT) == KEY_REPEAT;
			break;
		}
	}
	return false;
}

bool PlayerController::Backward(bool flipped) const
{
	if (!flipped)
	{
		switch (type)
		{
		case KEYBOARD:
			return game->input->GetKey(SDL_SCANCODE_LEFT) == KEY_DOWN || game->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT;
			break;
		case JOYSTICK:
			return game->input->GetJoy(JOY_LEFT) == KEY_REPEAT;
			break;
		}
	}
	else
	{
		switch (type)
		{
		case KEYBOARD:
			return game->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_DOWN || game->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT;
			break;
		case JOYSTICK:
			return game->input->GetJoy(JOY_RIGHT) == KEY_REPEAT;
			break;
		}
	}
	return false;
}

bool PlayerController::Down() const
{
	switch (type)
	{
	case KEYBOARD:
		return game->input->GetKey(SDL_SCANCODE_DOWN) == KEY_DOWN || game->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT;
		break;
	case JOYSTICK:
		return game->input->GetJoy(JOY_DOWN) == KEY_REPEAT;
		break;
	}
	return false;
}

bool PlayerController::Up() const
{
	switch (type)
	{
	case KEYBOARD:
		return game->input->GetKey(SDL_SCANCODE_UP) == KEY_DOWN || game->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT;
		break;
	case JOYSTICK:
		return game->input->GetJoy(JOY_UP) == KEY_REPEAT;
		break;
	}
	return false;
}

bool PlayerController::L_Punch() const
{
	switch (type)
	{
	case KEYBOARD:
		return game->input->GetKey(SDL_SCANCODE_A) == KEY_DOWN;
		break;
	case JOYSTICK:
		return game->input->GetJoy(JOY_BUTTON_X) == KEY_DOWN;
		break;
	}
	return false;
}

bool PlayerController::M_Punch() const
{
	switch (type)
	{
	case KEYBOARD:
		return game->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN;
		break;
	case JOYSTICK:
		return game->input->GetJoy(JOY_BUTTON_Y) == KEY_DOWN;
		break;
	}
	return false;
}

bool PlayerController::H_Punch() const
{
	switch (type)
	{
	case KEYBOARD:
		return game->input->GetKey(SDL_SCANCODE_D) == KEY_DOWN;
		break;
	case JOYSTICK:
		return game->input->GetJoy(JOY_BUTTON_LB) == KEY_DOWN;
		break;
	}
	return false;
}

bool PlayerController::L_Kick() const
{
	switch (type)
	{
	case KEYBOARD:
		return game->input->GetKey(SDL_SCANCODE_Z) == KEY_DOWN;
		break;
	case JOYSTICK:
		return game->input->GetJoy(JOY_BUTTON_A) == KEY_DOWN;
		break;
	}
	return false;
}

bool PlayerController::M_Kick() const
{
	switch (type)
	{
	case KEYBOARD:
		return game->input->GetKey(SDL_SCANCODE_X) == KEY_DOWN;
		break;
	case JOYSTICK:
		return game->input->GetJoy(JOY_BUTTON_B) == KEY_DOWN;
		break;
	}
	return false;
}

bool PlayerController::H_Kick() const
{
	switch (type)
	{
	case KEYBOARD:
		return game->input->GetKey(SDL_SCANCODE_C) == KEY_DOWN;
		break;
	case JOYSTICK:
		return game->input->GetJoy(JOY_BUTTON_RB) == KEY_DOWN;
		break;
	}
	return false;
}
