#ifndef __EDITOR_H_
#define __EDITOR_H_

#include <vector>
#include <string>

struct SDL_Window;
struct ImGuiIO;

class Sprite;
class AnimationSheet;
class Animation;
class Fx;

class Editor
{
public:
	bool Init();
	bool Update();
	bool PostUpdate();

	//members

	ImGuiIO* io;
	Sprite* spriteSheet = nullptr;
	Sprite* fxSpriteSheet = nullptr;
	AnimationSheet* as = nullptr;
	AnimationSheet* as2 = nullptr; //TODO: CLean
	Fx* fx = nullptr;
	Animation* animPreview = nullptr;
	Sprite* bg = nullptr;

	std::vector<std::string> files;
	std::vector<std::string> dirs;

	bool testing = false;
};

#endif