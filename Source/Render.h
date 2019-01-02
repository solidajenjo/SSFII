#ifndef __RENDER_H_
#define __RENDER_H_

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
	void RenderSprite(Sprite* sprite);
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