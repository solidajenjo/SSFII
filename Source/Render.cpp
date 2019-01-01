#include "Game.h"
#include "Globals.h"
#include "Render.h"
#include "ExternalLibraries/SDL/include/SDL.h"
#include "ExternalLibraries/imgui/imgui.h"
#include "ExternalLibraries/imgui/examples/imgui_impl_opengl3.h"
#include "ExternalLibraries/glew-2.1.0/include/GL/glew.h"


bool Render::Init()
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{			
		LOG("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
	}
	else
	{
		window = SDL_CreateWindow("Super Street Fighter II", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
		if (window == nullptr)
		{
			LOG("Window could not be created! SDL_Error: %s\n", SDL_GetError());
		}
		else
		{
			screenSurface = SDL_GetWindowSurface(window);
			SDL_FillRect(screenSurface, nullptr, SDL_MapRGB(screenSurface->format, 0xFF, 0xFF, 0xFF));
			SDL_UpdateWindowSurface(window);

		}
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

	context = SDL_GL_CreateContext(window);

	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		LOG("Error Initializating GLEW");
	}

	return true;
}

bool Render::PreUpdate()
{
	return false;
}

bool Render::Update()
{
	return true;
}

bool Render::PostUpdate()
{
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	SDL_RenderPresent(renderer);
	SDL_GL_SwapWindow(window);
	return true;
}

bool Render::Quit()
{
	if (renderer != nullptr)
	{
		SDL_DestroyRenderer(renderer);
	}
	SDL_FreeSurface(screenSurface);
	SDL_DestroyWindow(window);
	LOG("Render quit");
	return true;
}
