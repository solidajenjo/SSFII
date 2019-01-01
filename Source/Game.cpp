#include "Game.h"
#include "Render.h"
#include "Editor.h"
#include "Input.h"

bool Game::Init()
{
	render = new Render();
	editor = new Editor();
	input = new Input();

	render->Init();
	editor->Init();
	input->Init();
	return true;
}

bool Game::PreUpdate()
{
	bool status = input->PreUpdate();
	return status;
}

bool Game::Update()
{
	bool status = render->Update();
	status = status && editor->Update();
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
