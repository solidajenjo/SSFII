#ifndef __RENDER_H_
#define __RENDER_H_

#include "ExternalLibraries/MathGeoLib/include/Math/float3.h"

struct SDL_Window;
struct SDL_Surface;
struct SDL_Renderer;

class Sprite;

class Render
{
public:
	
	bool Init();
	bool PreUpdate();
	bool Update();
	void RenderSprite(const Sprite* sprite, const float3 &pos) const;
	bool PostUpdate();
	bool Quit();

	char* readFile(const char* name) const;

	//members

	SDL_Window* window = nullptr;
	SDL_Surface* screenSurface = nullptr;
	SDL_Renderer* renderer = nullptr;
	
	void* context;

	unsigned program = 0;

};


#endif