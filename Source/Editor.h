#ifndef __EDITOR_H_
#define __EDITOR_H_

struct SDL_Window;
struct ImGuiIO;

class Editor
{
public:
	bool Init();
	bool Update();
	bool PostUpdate();

	//members

	ImGuiIO* io;
};

#endif