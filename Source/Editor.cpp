#include "Editor.h"
#include "ExternalLibraries/imgui/imgui.h"
#include "ExternalLibraries/imgui/examples/imgui_impl_sdl.h"
#include "ExternalLibraries/SDL/include/SDL.h"
#include "ExternalLibraries/imgui/examples/imgui_impl_opengl3.h"
#include "Render.h"
#include "Game.h"
#include "Sprite.h"
#include "AnimationSheet.h"
#include "Animation.h"
#include "FileSystem.h"
#include "CharacterController.h"

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
	float2 previewPos = float2(100.f, 300.f);
	if (ImGui::Button("Test") && game->characterController != nullptr)
	{
		testing = true;
		animPreview = nullptr;
	}
	if (ImGui::CollapsingHeader("Animation Editor"))
	{
		ImGui::Text("Animation sheet name: ");
		if (as != nullptr)
		{
			ImGui::SameLine();
			char sheetName[256];
			sprintf_s(sheetName, as->sheetName.c_str());
			if (ImGui::InputText("Sheet Name", sheetName, 256))
				as->sheetName = std::string(sheetName);
		}

		if (ImGui::Button("New", ImVec2(100, 20)))
		{
			RELEASE(as);
			as = new AnimationSheet("default.sht");
			files.resize(0);
			dirs.resize(0);
			game->fileSystem->GetContentList("SpriteSheets", files, dirs);
			ImGui::OpenPopup("LoadSpriteSheetPopup");
		}
		ImGui::SameLine();
		if (ImGui::Button("Save", ImVec2(100, 20)))
		{
			as->Serialize();
		}
		ImGui::SameLine();
		if (ImGui::Button("Load", ImVec2(100, 20)))
		{
			files.resize(0);
			dirs.resize(0);
			game->fileSystem->GetContentList("AnimSheets", files, dirs);
			ImGui::OpenPopup("LoadPopup");				
		}
		if (ImGui::BeginPopup("LoadPopup", ImGuiWindowFlags_Modal))
		{
			for (std::string s : files)
			{
				if (ImGui::Button(s.c_str()))
				{
					RELEASE(spriteSheet);
					RELEASE(as);
					RELEASE(animPreview);
					as = new AnimationSheet(s.c_str());
					as->LoadSheet();
					spriteSheet = new Sprite(as->sheetPath);
					ImGui::CloseCurrentPopup();
					//TODO:Release old character controller
					game->characterController = new CharacterController(as, float3(previewPos, 0.f));
				}
			}
			ImGui::EndPopup();
		}
		if (ImGui::BeginPopup("LoadSpriteSheetPopup", ImGuiWindowFlags_Modal))
		{
			for (std::string s : files)
			{
				if (ImGui::Button(s.c_str()))
				{
					spriteSheet = new Sprite(std::string(("SpriteSheets/"+s).c_str()));
					as->sheetPath = std::string("SpriteSheets/" + s);
					ImGui::CloseCurrentPopup();
				}
			}
			ImGui::EndPopup();
		}
		
		if (spriteSheet != nullptr)
		{
			ImGui::BeginChildFrame(1, ImVec2(spriteSheet->width, 250));
			ImVec2 imagePos = ImVec2(ImGui::GetWindowPos().x + ImGui::GetCursorPos().x,
				ImGui::GetWindowPos().y + ImGui::GetCursorPos().y);
			ImGui::Image((void*)(intptr_t)spriteSheet->texture, ImVec2(spriteSheet->width, spriteSheet->height), ImVec2(0, 1), ImVec2(1, 0));
			if (ImGui::IsItemHovered())
			{
				int x = ImGui::GetMousePos().x - imagePos.x;
				int y = ImGui::GetMousePos().y - imagePos.y + ImGui::GetScrollY();
				ImGui::BeginTooltip();
				ImGui::Text("x: %d y: %d", x, y);
				ImGui::EndTooltip();
			}
			ImGui::EndChildFrame();
		}
		ImVec2 regAvail = ImGui::GetContentRegionAvail();
		ImGui::BeginChildFrame(2, ImVec2(regAvail.x, regAvail.y));	
		if (animPreview != nullptr)
		{
			ImGui::Text("Previewing - ");
			ImGui::SameLine();
			ImGui::Text(animPreview->name.c_str());
		}
		static bool play = false;
		if (as != nullptr)
		{
			for (unsigned i = 0u; i < ANIM_NUM; ++i)
			{
				if (ImGui::TreeNodeEx(as->ANIM_NAMES[i]))
				{
					ImGui::PushItemWidth(100.f);
					ImGui::PushID(i);
					
					int nFrames = as->animations[i]->nFrames;
					if (ImGui::Button("Preview"))
					{
						animPreview = as->animations[i];
						testing = false;
					}
					ImGui::SameLine();
					if (ImGui::Button("Play"))
					{
						play = true;
					}
					ImGui::SameLine();
					if (ImGui::Button("Stop"))
					{
						play = false;
					}
					if (ImGui::InputInt("# of frames", &nFrames))
					{
						if (nFrames > 0)
						{
							as->animations[i]->Reset(nFrames, as->sheetPath); 
						}
					}
					int frameDuration = as->animations[i]->frameDuration;
					ImGui::InputInt("Frame duration (ms)", &frameDuration);
					as->animations[i]->frameDuration = frameDuration;
					int currentFrame = as->animations[i]->currentFrame;
					if (ImGui::InputInt("Current Frame", &currentFrame))
					{
						if (currentFrame >= 0 && currentFrame < nFrames)
						{
							as->animations[i]->currentFrame = currentFrame;
						}						
					}
					ImGui::Separator();
					if (as->animations[i]->nFrames > 0)
					{
						if (ImGui::Button("Propagate frame 0 coordinates"))
						{
							for (unsigned j = 1u; j < nFrames; ++j)
							{
								as->animations[i]->frames[j]->sprite->x = as->animations[i]->frames[0]->sprite->x;
								as->animations[i]->frames[j]->sprite->y = as->animations[i]->frames[0]->sprite->y;
								as->animations[i]->frames[j]->sprite->width = as->animations[i]->frames[0]->sprite->width;
								as->animations[i]->frames[j]->sprite->height = as->animations[i]->frames[0]->sprite->height;
								as->animations[i]->frames[j]->sprite->Flush();
								as->animations[i]->frames[j]->sprite->CreateSprite();
							}
						}
						if (ImGui::InputInt("X", &as->animations[i]->frames[as->animations[i]->currentFrame]->sprite->x) ||
							ImGui::InputInt("Y", &as->animations[i]->frames[as->animations[i]->currentFrame]->sprite->y) ||
							ImGui::InputInt("W", &as->animations[i]->frames[as->animations[i]->currentFrame]->sprite->width) ||
							ImGui::InputInt("H", &as->animations[i]->frames[as->animations[i]->currentFrame]->sprite->height))
						{
							as->animations[i]->frames[as->animations[i]->currentFrame]->sprite->Flush();
							as->animations[i]->frames[as->animations[i]->currentFrame]->sprite->CreateSprite();
						}
						ImGui::InputInt("Offset H", &as->animations[i]->frames[as->animations[i]->currentFrame]->offsetH);
						ImGui::InputInt("Offset V", &as->animations[i]->frames[as->animations[i]->currentFrame]->offsetV);
						ImGui::Separator();
						ImGui::Text("Hit boxes");
						if (ImGui::Button("Propagate frame 0 hit boxes"))
						{
							for (unsigned j = 1u; j < nFrames; ++j)
								for (unsigned jj = 0u; jj < 4u; ++jj)
									as->animations[i]->frames[j]->hitBoxes[jj] = as->animations[i]->frames[0]->hitBoxes[jj];
						}
						ImGui::Text("Face");
						ImGui::PushID("Face");
						ImGui::InputFloat("MinPoint X", &as->animations[i]->frames[as->animations[i]->currentFrame]->hitBoxes[0].box.minPoint[0]);
						ImGui::InputFloat("MinPoint Y", &as->animations[i]->frames[as->animations[i]->currentFrame]->hitBoxes[0].box.minPoint[1]);

						ImGui::InputFloat("MaxPoint X", &as->animations[i]->frames[as->animations[i]->currentFrame]->hitBoxes[0].box.maxPoint[0]);
						ImGui::InputFloat("MaxPoint Y", &as->animations[i]->frames[as->animations[i]->currentFrame]->hitBoxes[0].box.maxPoint[1]);
						
						ImGui::PopID();
						
						ImGui::Text("Body");
						ImGui::PushID("Body");
						
						ImGui::InputFloat("MinPoint X", &as->animations[i]->frames[as->animations[i]->currentFrame]->hitBoxes[1].box.minPoint[0]);
						ImGui::InputFloat("MinPoint Y", &as->animations[i]->frames[as->animations[i]->currentFrame]->hitBoxes[1].box.minPoint[1]);

						ImGui::InputFloat("MaxPoint X", &as->animations[i]->frames[as->animations[i]->currentFrame]->hitBoxes[1].box.maxPoint[0]);
						ImGui::InputFloat("MaxPoint Y", &as->animations[i]->frames[as->animations[i]->currentFrame]->hitBoxes[1].box.maxPoint[1]);

						ImGui::PopID();

						ImGui::Text("Legs");
						ImGui::PushID("Legs");
						
						ImGui::InputFloat("MinPoint X", &as->animations[i]->frames[as->animations[i]->currentFrame]->hitBoxes[2].box.minPoint[0]);
						ImGui::InputFloat("MinPoint Y", &as->animations[i]->frames[as->animations[i]->currentFrame]->hitBoxes[2].box.minPoint[1]);

						ImGui::InputFloat("MaxPoint X", &as->animations[i]->frames[as->animations[i]->currentFrame]->hitBoxes[2].box.maxPoint[0]);
						ImGui::InputFloat("MaxPoint Y", &as->animations[i]->frames[as->animations[i]->currentFrame]->hitBoxes[2].box.maxPoint[1]);

						game->render->DrawBox(previewPos + as->animations[i]->frames[as->animations[i]->currentFrame]->hitBoxes[2].box.minPoint,
							previewPos + as->animations[i]->frames[as->animations[i]->currentFrame]->hitBoxes[2].box.maxPoint);
						ImGui::PopID();

						ImGui::Text("Attack");
						ImGui::PushID("Attack");

						ImGui::InputInt("Damage amount", &as->animations[i]->frames[as->animations[i]->currentFrame]->hitBoxes[3].damageAmount);
						ImGui::InputFloat("MinPoint X", &as->animations[i]->frames[as->animations[i]->currentFrame]->hitBoxes[3].box.minPoint[0]);
						ImGui::InputFloat("MinPoint Y", &as->animations[i]->frames[as->animations[i]->currentFrame]->hitBoxes[3].box.minPoint[1]);

						ImGui::InputFloat("MaxPoint X", &as->animations[i]->frames[as->animations[i]->currentFrame]->hitBoxes[3].box.maxPoint[0]);
						ImGui::InputFloat("MaxPoint Y", &as->animations[i]->frames[as->animations[i]->currentFrame]->hitBoxes[3].box.maxPoint[1]);
						
						ImGui::PopID();

					}
					if (animPreview != nullptr)
					{
						ImGui::Text(animPreview->name.c_str());
						if (animPreview->nFrames > 0 && animPreview->frames[animPreview->currentFrame]->sprite != nullptr)
						{
							if (!play)
								game->render->RenderSprite(animPreview->frames[animPreview->currentFrame]->sprite, 
									float3(previewPos, 0.f) + float3(animPreview->frames[animPreview->currentFrame]->offsetH,
										animPreview->frames[animPreview->currentFrame]->offsetV, .0f));
							else
							{
								bool loopEnded;
								animPreview->Play(float3(previewPos, 0.f), loopEnded);
							}
							game->render->DrawBox(previewPos + animPreview->frames[animPreview->currentFrame]->hitBoxes[0].box.minPoint,
								previewPos + animPreview->frames[animPreview->currentFrame]->hitBoxes[0].box.maxPoint);
							game->render->DrawBox(previewPos + animPreview->frames[animPreview->currentFrame]->hitBoxes[1].box.minPoint,
								previewPos + animPreview->frames[animPreview->currentFrame]->hitBoxes[1].box.maxPoint);
							game->render->DrawBox(previewPos + animPreview->frames[animPreview->currentFrame]->hitBoxes[2].box.minPoint,
								previewPos + animPreview->frames[animPreview->currentFrame]->hitBoxes[2].box.maxPoint);
							game->render->DrawBox(previewPos + animPreview->frames[animPreview->currentFrame]->hitBoxes[3].box.minPoint,
								previewPos + animPreview->frames[animPreview->currentFrame]->hitBoxes[3].box.maxPoint, true);
						}
					}
					ImGui::PopID();
					ImGui::TreePop();
				}				
			}
		}
		ImGui::EndChildFrame();
		ImGui::SameLine();
		
	}
	

	return true;
}

bool Editor::PostUpdate()
{
	ImGui::Render();
	return true;
}

