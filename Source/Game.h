#ifndef __GAME_H_
#define __GAME_H_

class Editor;
class Render;
class Input;

class Game
{
public:
	bool Init();
	bool PreUpdate();
	bool Update();
	bool PostUpdate();
	bool Quit();

	//members

	Render* render = nullptr;
	Editor* editor = nullptr;
	Input* input = nullptr;
};

extern Game* game;
#endif