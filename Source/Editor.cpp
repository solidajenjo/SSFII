#include "Editor.h"
#include "ExternalLibraries/imgui/imgui.h"
#include "ExternalLibraries/imgui/examples/imgui_impl_sdl.h"
#include "ExternalLibraries/SDL/include/SDL.h"
#include "ExternalLibraries/imgui/examples/imgui_impl_opengl3.h"
#include "Render.h"
#include "Game.h"
#include "Sprite.h"
#include "AnimationSheet.h"
#include "Fx.h"
#include "Animation.h"
#include "FileSystem.h"
#include "CharacterController.h"
#include "AI.h"
#include "AIManager.h"

bool Editor::Init()
{
	
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	io = &ImGui::GetIO(); (void)io;

	ImGui_ImplSDL2_InitForOpenGL(game->render->window, game->render->context);
	ImGui_ImplOpenGL3_Init("#version 330");

	ImGui::StyleColorsDark();

	bg = new Sprite("BackGrounds/testBG.gif");
	fitness.push_back(0.f);
	return true;
}

bool Editor::Update()
{	
	game->render->RenderSprite(bg, float3(SCREEN_WIDTH * 0.5f, 0.f, 0.f), 1.6f, 0.f, 0.f, false, float3::one);
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(game->render->window);
	ImGui::NewFrame();

	if (testing)
	{
		if (rand() % 100 < 50 || game->characterController1->landingWaitTimer > 0)
		{
			game->aiManager->ai1->Update();			
		}
		if (rand() % 100 < 50 || game->characterController2->landingWaitTimer > 0)
		{
			game->aiManager->ai2->Update();
		}
		ImGui::Text("Generation %d", generation);
		ImGui::InputInt("Mutation chance %", &mutationPosibiliy);
		ImGui::Text("Life A = %d - Life B = %d", game->characterController1->life, game->characterController2->life);
		ImGui::Text("NN Input A = %s - NN Input B = %s", game->characterController1->neuralNetworkInput.c_str(), 
			game->characterController2->neuralNetworkInput.c_str());		
		ImGui::PlotLines("Fitness evo", &fitness[0], fitness.size(), 0, "Fitness evo", -1000000.f, 1000000.f, ImVec2(ImGui::GetWindowContentRegionMax().x, 60));
		/*ImGui::InputFloat("Block Prize", &AI::blockPrize, 0.1f, 1.f);
		ImGui::InputFloat("Walk Prize", &AI::walkPrize, 0.1f, 1.f);
		ImGui::InputFloat("Distance attack penalization", &AI::attackDistancePenalization, 0.1f, 1.f, "%.6f");*/
		game->aiManager->ai1->Text();
		game->aiManager->ai2->Text();		
		unsigned timeRemaining = (endRound - SDL_GetTicks()) / 1000;
		ImGui::InputInt("Round time", &roundDuration);
		ImGui::Text("Time remaining %d", timeRemaining);
		ImGui::Text("%d - %s (%s)(%s) vs %d - %s (%s)(%s)", ai1Num, game->aiManager->ai1->name, ((AI*)game->characterController1->controller)->name, ((AI*)game->aiManager->ai1->other->controller)->name,
			ai2Num, game->aiManager->ai2->name, ((AI*)game->characterController2->controller)->name, ((AI*)game->aiManager->ai2->other->controller)->name);
		if (SDL_GetTicks() >= endRound || game->characterController1->state == CharacterController::CharacterStates::KO 
			|| game->characterController2->state == CharacterController::CharacterStates::KO)
		{
			game->aiManager->Train();
			endRound = SDL_GetTicks() + roundDuration;
		}
	}
	if (ImGui::Button("Test") && game->characterController1 != nullptr)
	{
		testing = true;
		ai1Num = 0;
		ai2Num = 1;
		game->aiManager->StartTrainning();
		animPreview = nullptr;
	}
	ImGui::SameLine();
	if (ImGui::Button("Load AI", ImVec2(100, 20)))
	{
		files.resize(0);
		dirs.resize(0);
		game->fileSystem->GetContentList("Brains", files, dirs);
		ImGui::OpenPopup("LoadAIPopup");
	}
	if (ImGui::BeginPopup("LoadAIPopup", ImGuiWindowFlags_Modal))
	{
		unsigned i = 1u;
		for (std::string s : files)
		{
			if (ImGui::Button(s.c_str()))
			{
				ImGui::CloseCurrentPopup();
				game->aiManager->Load(s);				
			}
			if (i % 10 != 0)
				ImGui::SameLine();
			++i;
		}
		ImGui::EndPopup();
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
					as2 = new AnimationSheet(s.c_str());
					as2->LoadSheet();
					spriteSheet = new Sprite(as->sheetPath);
					ImGui::CloseCurrentPopup();
					//TODO:Release old character controller					
					game->characterController1 = new CharacterController(as, float3(previewPos, 0.f));
					game->characterController1->controller = game->keyboardController;					
					game->characterController2 = new CharacterController(as2, float3(previewPos2, 0.f));
					game->characterController2->controller = game->joystickController;					
					game->characterController2->other = game->characterController1;
					game->characterController1->other = game->characterController2;
					
					endRound = SDL_GetTicks() + roundDuration;
					nextUpdate = SDL_GetTicks() + updateWait;
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
					ImGui::InputFloat("Scale", &as->animations[i]->scale);
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
						ImGui::InputFloat("MinPoint X", &as->animations[i]->frames[as->animations[i]->currentFrame]->hitBoxes[0].box.minPoint[0], 2.f);
						ImGui::InputFloat("MinPoint Y", &as->animations[i]->frames[as->animations[i]->currentFrame]->hitBoxes[0].box.minPoint[1], 2.f);

						ImGui::InputFloat("MaxPoint X", &as->animations[i]->frames[as->animations[i]->currentFrame]->hitBoxes[0].box.maxPoint[0], 2.f);
						ImGui::InputFloat("MaxPoint Y", &as->animations[i]->frames[as->animations[i]->currentFrame]->hitBoxes[0].box.maxPoint[1], 2.f);
						
						ImGui::PopID();
						
						ImGui::Text("Body");
						ImGui::PushID("Body");
						
						ImGui::InputFloat("MinPoint X", &as->animations[i]->frames[as->animations[i]->currentFrame]->hitBoxes[1].box.minPoint[0], 2.f);
						ImGui::InputFloat("MinPoint Y", &as->animations[i]->frames[as->animations[i]->currentFrame]->hitBoxes[1].box.minPoint[1], 2.f);

						ImGui::InputFloat("MaxPoint X", &as->animations[i]->frames[as->animations[i]->currentFrame]->hitBoxes[1].box.maxPoint[0], 2.f);
						ImGui::InputFloat("MaxPoint Y", &as->animations[i]->frames[as->animations[i]->currentFrame]->hitBoxes[1].box.maxPoint[1], 2.f);

						ImGui::PopID();

						ImGui::Text("Attack");
						ImGui::PushID("Attack");
						
						ImGui::InputFloat("MinPoint X", &as->animations[i]->frames[as->animations[i]->currentFrame]->hitBoxes[2].box.minPoint[0], 2.f);
						ImGui::InputFloat("MinPoint Y", &as->animations[i]->frames[as->animations[i]->currentFrame]->hitBoxes[2].box.minPoint[1], 2.f);
																															  
						ImGui::InputFloat("MaxPoint X", &as->animations[i]->frames[as->animations[i]->currentFrame]->hitBoxes[2].box.maxPoint[0], 2.f);
						ImGui::InputFloat("MaxPoint Y", &as->animations[i]->frames[as->animations[i]->currentFrame]->hitBoxes[2].box.maxPoint[1], 2.f);
						
						ImGui::PopID();

					}
					if (animPreview != nullptr)
					{
						ImGui::Text(animPreview->name.c_str());
						if (animPreview->nFrames > 0 && animPreview->frames[animPreview->currentFrame]->sprite != nullptr)
						{
							float2 offsettedPos = float2(previewPos) + float2(animPreview->frames[animPreview->currentFrame]->offsetH,
								animPreview->frames[animPreview->currentFrame]->offsetV);
							float2 offsettedPos2 = float2(previewPos2) - float2(animPreview->frames[animPreview->currentFrame]->offsetH,
								-animPreview->frames[animPreview->currentFrame]->offsetV);
							if (!play)
							{
								animPreview->UpdateHBoxes(previewPos, false);
								game->render->RenderSprite(animPreview->frames[animPreview->currentFrame]->sprite,
									float3(offsettedPos, 0.f), animPreview->scale, 0.f, 0.f, false, animPreview->color );
								animPreview->DrawHBoxes();
								animPreview->UpdateHBoxes(previewPos2, true);
								game->render->RenderSprite(animPreview->frames[animPreview->currentFrame]->sprite,
									float3(offsettedPos2, 0.f), animPreview->scale, 0.f, 0.f, true, animPreview->color);
								animPreview->DrawHBoxes();
							}
							else
							{
								bool loopEnded;
								animPreview->UpdateHBoxes(previewPos, false);
								animPreview->Play(float3(previewPos, 0.f), loopEnded, false, float3::one);
								animPreview->DrawHBoxes();
								animPreview->UpdateHBoxes(previewPos2, true);
								animPreview->Play(float3(previewPos2, 0.f), loopEnded, true, float3::one);
								animPreview->DrawHBoxes();
							}
							
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
	
	if (ImGui::CollapsingHeader("Fx Editor"))
	{
		if (fx != nullptr)
		{
			char sheetName[256];
			sprintf_s(sheetName, fx->sheetName.c_str());
			if (ImGui::InputText("Sheet Name", sheetName, 256))
				fx->sheetName = std::string(sheetName);
		}
		if (ImGui::Button("New", ImVec2(100, 20)))
		{
			RELEASE(fx);
			fx = new Fx("default.fx");
			files.resize(0);
			dirs.resize(0);
			game->fileSystem->GetContentList("SpriteSheets", files, dirs);
			ImGui::OpenPopup("LoadFxSheetPopup");
		}
		ImGui::SameLine();
		if (ImGui::Button("Save", ImVec2(100, 20)))
		{
			fx->Serialize();
		}
		ImGui::SameLine();
		if (ImGui::Button("Load", ImVec2(100, 20)))
		{
			files.resize(0);
			dirs.resize(0);
			game->fileSystem->GetContentList("Fx", files, dirs);
			ImGui::OpenPopup("LoadFxPopup");
		}
		if (ImGui::BeginPopup("LoadFxPopup", ImGuiWindowFlags_Modal))
		{
			for (std::string s : files)
			{
				if (ImGui::Button(s.c_str()))
				{
					RELEASE(spriteSheet);
					RELEASE(as);
					RELEASE(animPreview);
					fx = new Fx(s.c_str());
					fx->LoadSheet();
					fxSpriteSheet = new Sprite(fx->sheetPath);
					ImGui::CloseCurrentPopup();					
				}
			}
			ImGui::EndPopup();
		}
		if (ImGui::BeginPopup("LoadFxSheetPopup", ImGuiWindowFlags_Modal))
		{
			for (std::string s : files)
			{
				if (ImGui::Button(s.c_str()))
				{
					fxSpriteSheet = new Sprite(std::string(("SpriteSheets/" + s).c_str()));
					fx->sheetPath = std::string("SpriteSheets/" + s);
					ImGui::CloseCurrentPopup();
				}
			}
			ImGui::EndPopup();
		}
		ImGui::PushItemWidth(100.f);
		if (fxSpriteSheet != nullptr)
		{
			ImGui::BeginChildFrame(3, ImVec2(fxSpriteSheet->width, 250));
			ImVec2 imagePos = ImVec2(ImGui::GetWindowPos().x + ImGui::GetCursorPos().x,
				ImGui::GetWindowPos().y + ImGui::GetCursorPos().y);
			ImGui::Image((void*)(intptr_t)fxSpriteSheet->texture, ImVec2(fxSpriteSheet->width, fxSpriteSheet->height), ImVec2(0, 1), ImVec2(1, 0));
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
		if (fx != nullptr)
		{
			static bool playFx = false;
			if (ImGui::Button("Play"))
			{
				playFx = true;
			}
			ImGui::SameLine();
			if (ImGui::Button("Stop"))
			{
				playFx = false;
			}
			int nFrames = fx->animation->nFrames;
			if (ImGui::InputInt("# of frames", &nFrames))
			{
				if (nFrames > 0)
				{
					fx->animation->Reset(nFrames, fx->sheetPath);
				}
			}
			int frameDuration = fx->animation->frameDuration;
			ImGui::InputInt("Frame duration (ms)", &frameDuration);
			fx->animation->frameDuration = frameDuration;
			int currentFrame = fx->animation->currentFrame;
			if (ImGui::InputInt("Current Frame", &currentFrame))
			{
				if (currentFrame >= 0 && currentFrame < nFrames)
				{
					fx->animation->currentFrame = currentFrame;
				}
			}
			ImGui::InputFloat("Scale", &fx->animation->scale);
			ImGui::Separator();
			if (fx->animation->nFrames > 0u)
			{
				ImGui::BeginChildFrame(4, ImVec2(regAvail.x, regAvail.y));
				ImGui::PushItemWidth(100.f);
				if (ImGui::InputInt("X", &fx->animation->frames[fx->animation->currentFrame]->sprite->x) ||
					ImGui::InputInt("Y", &fx->animation->frames[fx->animation->currentFrame]->sprite->y) ||
					ImGui::InputInt("W", &fx->animation->frames[fx->animation->currentFrame]->sprite->width) ||
					ImGui::InputInt("H", &fx->animation->frames[fx->animation->currentFrame]->sprite->height))
				{
					fx->animation->frames[fx->animation->currentFrame]->sprite->Flush();
					fx->animation->frames[fx->animation->currentFrame]->sprite->CreateSprite();
				}
				ImGui::InputInt("Offset H", &fx->animation->frames[fx->animation->currentFrame]->offsetH);
				ImGui::InputInt("Offset V", &fx->animation->frames[fx->animation->currentFrame]->offsetV);
				ImGui::Separator();
				ImGui::Text("Hit box");
				ImGui::InputFloat("MinPoint X", &fx->animation->frames[fx->animation->currentFrame]->hitBoxes[0].box.minPoint[0], 2.f);
				ImGui::InputFloat("MinPoint Y", &fx->animation->frames[fx->animation->currentFrame]->hitBoxes[0].box.minPoint[1], 2.f);

				ImGui::InputFloat("MaxPoint X", &fx->animation->frames[fx->animation->currentFrame]->hitBoxes[0].box.maxPoint[0], 2.f);
				ImGui::InputFloat("MaxPoint Y", &fx->animation->frames[fx->animation->currentFrame]->hitBoxes[0].box.maxPoint[1], 2.f);
				ImGui::PopItemWidth();
				ImGui::EndChildFrame();
				float2 offsettedPos = float2(previewPos) + float2(fx->animation->frames[fx->animation->currentFrame]->offsetH,
					fx->animation->frames[fx->animation->currentFrame]->offsetV);
				float2 offsettedPos2 = float2(previewPos2) - float2(fx->animation->frames[fx->animation->currentFrame]->offsetH,
					-fx->animation->frames[fx->animation->currentFrame]->offsetV);
				if (!playFx)
				{
					fx->animation->UpdateHBoxes(previewPos, false);
					game->render->RenderSprite(fx->animation->frames[fx->animation->currentFrame]->sprite,
						float3(offsettedPos, 0.f), fx->animation->scale, 0.f, 0.f, false, float3::one);
					fx->animation->DrawHBoxes();					
				}
				else
				{
					bool loopEnded;
					fx->animation->UpdateHBoxes(previewPos, false);
					fx->animation->Play(float3(previewPos, 0.f), loopEnded, false, float3::one);
					fx->animation->DrawHBoxes();					
				}
			}
		}
		ImGui::PopItemWidth();
	}

	return true;
}

bool Editor::PostUpdate()
{
	ImGui::Render();
	return true;
}

