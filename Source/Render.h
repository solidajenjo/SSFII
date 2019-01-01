#ifndef __RENDER_H_
#define __RENDER_H_

struct SDL_Window;
struct SDL_Surface;
struct SDL_Renderer;

class Render
{
public:
	
	bool Init();
	bool PreUpdate();
	bool Update();
	bool PostUpdate();
	bool Quit();

	//members

	SDL_Window* window = nullptr;
	SDL_Surface* screenSurface = nullptr;
	SDL_Renderer* renderer = nullptr;
	
	void* context;

};


#endif