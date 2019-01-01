#include "Editor.h"
#include "ExternalLibraries/imgui/imgui.h"
#include "ExternalLibraries/imgui/examples/imgui_impl_sdl.h"
#include "ExternalLibraries/SDL/include/SDL.h"
#include "ExternalLibraries/imgui/examples/imgui_impl_opengl3.h"
#include "Render.h"
#include "Game.h"

bool Editor::Init()
{
	
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	io = &ImGui::GetIO(); (void)io;

	ImGui_ImplSDL2_InitForOpenGL(game->render->window, game->render->context);
	ImGui_ImplOpenGL3_Init("#version 330");

	ImGui::StyleColorsDark();

	return true;
}

bool Editor::Update()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(game->render->window);
	ImGui::NewFrame();

	if (ImGui::CollapsingHeader("Animation Editor"))
	{
		ImGui::Text("Lorem ipsum y todas esas mierdas...");
	}
	

	return true;
}

bool Editor::PostUpdate()
{
	ImGui::Render();
	return true;
}

