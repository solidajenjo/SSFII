#ifndef __GAME_H_
#define __GAME_H_

class Editor;
class Render;
class Input;
class Textures;
class FileSystem;

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
	Textures* textures = nullptr;
	FileSystem* fileSystem = nullptr;
};

extern Game* game;
#endif