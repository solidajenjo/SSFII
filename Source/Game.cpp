#include "Game.h"
#include "Globals.h"
#include "Render.h"
#include "Editor.h"
#include "Input.h"
#include "Textures.h"
#include "Sprite.h"
#include "Animation.h"
#include "AnimationSheet.h"
#include "CharacterController.h"
#include "PlayerController.h"
#include "FileSystem.h"

bool Game::Init()
{
	render = new Render();
	editor = new Editor();
	input = new Input();
	textures = new Textures();
	fileSystem = new FileSystem();
	joystickController = new PlayerController(PlayerControllerType::JOYSTICK);
	keyboardController = new PlayerController(PlayerControllerType::KEYBOARD); //TODO: Clean
	render->Init();
	editor->Init();
	input->Init();
	textures->Init();
	fileSystem->Init();

	return true;
}

bool Game::PreUpdate()
{
	bool status = input->PreUpdate();
	status = status && render->PreUpdate();	
	return status;
}

bool Game::Update()
{
	bool status = render->Update();
	status = status && editor->Update();
	if (characterController != nullptr && editor->testing)
	{
		characterController->Update();
	}
	return status;
}

bool Game::PostUpdate()
{
	bool status = editor->PostUpdate();
	status = status && render->PostUpdate();
	return status;
}

bool Game::Quit()
{
	render->Quit();
	input->Quit();
	fileSystem->CleanUp();
	return true;
}
