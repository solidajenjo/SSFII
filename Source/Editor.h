#ifndef __EDITOR_H_
#define __EDITOR_H_

#include <vector>
#include <string>
#include "ExternalLibraries/MathGeoLib/include/Math/float2.h"

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
	int roundDuration = 10000u;
	unsigned nextUpdate = 0u;
	unsigned updateWait = 300u;
	int mutationPosibiliy = 5;
	int generation = 0;
	std::vector<float> fitness;


	unsigned ai1Num = 0u;
	unsigned ai2Num = 1u;
	float2 previewPos = float2(400.f, 10.f);
	float2 previewPos2 = float2(800.f, 10.f);

	std::vector<std::string> files;
	std::vector<std::string> dirs;

	bool testing = false;
	
};

#endif