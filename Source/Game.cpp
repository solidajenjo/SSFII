#include "Game.h"
#include "Globals.h"
#include "Render.h"
#include "Editor.h"
#include "Input.h"
#include "Textures.h"
#include "Sprite.h"
#include "Animation.h"

bool Game::Init()
{
	render = new Render();
	editor = new Editor();
	input = new Input();
	textures = new Textures();
	render->Init();
	editor->Init();
	input->Init();
	textures->Init();
	animation = new Animation(2);
	animation->SetFrame(0, new Sprite("SpriteSheets\\blanka.png", 516, 234, 123, 119));
	animation->SetFrame(1, new Sprite("SpriteSheets\\blanka.png", 639, 234, 113, 119));
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
	bool b;
	animation->Play(float3(100,100,0), b);
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
	RELEASE(animation);
	return true;
}
