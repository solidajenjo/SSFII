#ifndef __GAME_H_
#define __GAME_H_

class Editor;
class Render;
class Input;
class Textures;
class FileSystem;
class CharacterController;
class PlayerController;

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
	CharacterController* characterController1 = nullptr;
	CharacterController* characterController2 = nullptr;
	PlayerController* joystickController = nullptr;
	PlayerController* keyboardController = nullptr;
};

extern Game* game;
#endif