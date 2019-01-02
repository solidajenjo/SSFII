#include "Game.h"
#include "Render.h"
#include "Editor.h"
#include "Input.h"
#include "Textures.h"
#include "Sprite.h"

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
	Sprite sprite("SpriteSheets\\blanka.png", 516, 234, 123, 119);
	render->RenderSprite(&sprite);
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
	return true;
}
