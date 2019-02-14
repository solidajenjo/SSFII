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
class AI;

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

	unsigned endRound = 0u;
	int roundDuration = 3000u;
	unsigned nextUpdate = 0u;
	unsigned updateWait = 300u;
	int mutationPosibiliy = 5;
	std::vector<float> fitness;

	AI* ai1 = nullptr;
	AI* ai2 = nullptr;

	unsigned ai1Num = 0u;
	unsigned ai2Num = 1u;

	std::vector<std::string> files;
	std::vector<std::string> dirs;

	bool testing = false;
	
};

#endif