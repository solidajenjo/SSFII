#ifndef __EDITOR_H_
#define __EDITOR_H_

#include <vector>
#include <string>

struct SDL_Window;
struct ImGuiIO;

class Sprite;
class AnimationSheet;
class Animation;

class Editor
{
public:
	bool Init();
	bool Update();
	bool PostUpdate();

	//members

	ImGuiIO* io;
	Sprite* spriteSheet = nullptr;
	AnimationSheet* as = nullptr;
	Animation* animPreview = nullptr;

	std::vector<std::string> files;
	std::vector<std::string> dirs;
};

#endif