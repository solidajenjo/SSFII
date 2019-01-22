#include "Editor.h"
#include "ExternalLibraries/imgui/imgui.h"
#include "ExternalLibraries/imgui/examples/imgui_impl_sdl.h"
#include "ExternalLibraries/SDL/include/SDL.h"
#include "ExternalLibraries/imgui/examples/imgui_impl_opengl3.h"
#include "Render.h"
#include "Game.h"
#include "Sprite.h"
#include "AnimationSheet.h"

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
		char sheetName[40] = "";
		char animSheetName[40] = "";
		ImGui::InputText("Animation sheet name", &animSheetName[0], 40); //TODO: Create dropdown with all the sheets created
		ImGui::InputText("Sheet name", &sheetName[0], 40);
		if (ImGui::Button("New", ImVec2(100, 20)))
		{

		}
		ImGui::SameLine();
		if (ImGui::Button("Save", ImVec2(100, 20)))
		{

		}
		ImGui::SameLine();
		if (ImGui::Button("Load", ImVec2(100, 20)))
		{

		}
		spriteSheet = new Sprite("SpriteSheets/ZangiefSS.gif");
		ImGui::BeginChildFrame(1, ImVec2(spriteSheet->width, 250));
		AnimationSheet as("");
		if (spriteSheet != nullptr)
			ImGui::Image((void*)(intptr_t)spriteSheet->texture, ImVec2(spriteSheet->width, spriteSheet->height), ImVec2(0, 1), ImVec2(1, 0));
		ImGui::EndChildFrame();
		ImVec2 regAvail = ImGui::GetContentRegionAvail();
		ImGui::BeginChildFrame(2, ImVec2(regAvail.x * .5f, regAvail.y));
		for (unsigned i = 0u; i < ANIM_NUM; ++i)
		{
			if (ImGui::TreeNodeEx(as.ANIM_NAMES[i].c_str()))
			{
				ImGui::TreePop();
			}
		}
		ImGui::EndChildFrame();
		ImGui::SameLine();
		ImGui::BeginChildFrame(3, ImVec2(regAvail.x * .5f, regAvail.y));
		ImGui::Text("Preview");
		ImGui::EndChildFrame();
	}
	

	return true;
}

bool Editor::PostUpdate()
{
	ImGui::Render();
	return true;
}

