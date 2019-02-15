#include "AI.h"
#include "AIManager.h"
#include "Game.h"
#include "FileSystem.h"
#include "CharacterController.h"
#include "ExternalLibraries/SDL/include/SDL_timer.h"
#include "Editor.h"

void AIManager::Save(std::string &name) const
{
	game->fileSystem->Write(name, &dojo, sizeof(dojo));
}

void AIManager::Load(std::string &name)
{
	game->fileSystem->Read("Brains/" + name, &dojo, sizeof(dojo));
	game->characterController1->life = game->characterController1->lifeAmount;
	game->characterController1->pos = float3(game->editor->previewPos, 1.f);
	game->characterController1->state = CharacterController::CharacterStates::IDLE;
	game->characterController1->isGrounded = true;
	game->characterController2->life = game->characterController2->lifeAmount;
	game->characterController2->pos = float3(game->editor->previewPos2, 1.f);
	game->characterController2->state = CharacterController::CharacterStates::IDLE;
	game->characterController2->isGrounded = true;
	
	game->editor->generation = dojo.generation + 1;

	for (unsigned i = 0u; i < AI_AMOUNT; ++i)
	{
		dojo.fighters[i].fitness = 0;
	}
	NextGeneration(game->editor->mutationPosibiliy);
	StartTrainning();
}

void AIManager::NextGeneration(unsigned mutationChance)
{
	unsigned mutantIndex = AI_AMOUNT / 3;
	unsigned crossedIndex = mutantIndex * 2;
	unsigned randomIndex = crossedIndex + mutantIndex / 2;
	
	unsigned gen = dojo.generation;
	unsigned j = 0u;

	for (unsigned i = mutantIndex; i < crossedIndex; ++i)
	{
		dojo.fighters[i].Mutate(dojo.fighters[rand() % mutantIndex].brain, mutationChance);
		sprintf_s(dojo.fighters[i].name, (std::string("GM") + std::to_string(gen) + std::string("_") + std::to_string(j++)).c_str());
	}
	for (unsigned i = crossedIndex; i < randomIndex; ++i)
	{
		unsigned b1, b2;
		b1 = b2 = rand() % mutantIndex;
		while (b1 == b2)
		{
			b2 = rand() % mutantIndex;
		}

		dojo.fighters[i].Cross(dojo.fighters[b1].brain, dojo.fighters[b2].brain);
		sprintf_s(dojo.fighters[i].name, (std::string("GC") + std::to_string(gen) + std::string("_") + std::to_string(j++)).c_str());
	}
	for (unsigned i = randomIndex; i < AI_AMOUNT; ++i)
	{
		dojo.fighters[i].Mutate(dojo.fighters[rand() % mutantIndex].brain, 100);
		sprintf_s(dojo.fighters[i].name, (std::string("GR") + std::to_string(gen) + std::string("_") + std::to_string(j++)).c_str());
	}
	dojo.generation = game->editor->generation;
}

void AIManager::StartTrainning()
{
	game->editor->ai1Num = 0u;
	game->editor->ai2Num = 1u;

	ai1 = dojo.fighters[game->editor->ai1Num];
	ai1.own = game->characterController1;
	game->characterController1->controller = (AI*)&ai1;
	ai1.other = game->characterController2;

	ai2 = dojo.fighters[game->editor->ai2Num];
	ai2.own = game->characterController2;
	game->characterController2->controller = (AI*)&ai2;
	ai2.other = game->characterController1;
}

void AIManager::Train() 
{
	dojo.fighters[game->editor->ai1Num].fitness += game->characterController1->life - game->characterController2->life;
	dojo.fighters[game->editor->ai2Num].fitness += game->characterController2->life - game->characterController1->life;
	ai1.fitness = dojo.fighters[game->editor->ai1Num].fitness;
	ai2.fitness = dojo.fighters[game->editor->ai2Num].fitness;
	game->editor->endRound = SDL_GetTicks() + game->editor->roundDuration;
	game->characterController1->flip = false;
	game->characterController2->flip = true;
	if (game->editor->ai2Num < (AI_AMOUNT - 1)) 
	{
		++game->editor->ai2Num;
		CharacterController* own = ai2.own;
		CharacterController* other = ai2.other;
		ai2 = dojo.fighters[game->editor->ai2Num];
		ai2.own = own;
		own->controller = &ai2;
		ai2.other = other;
	}
	else
	{
		CharacterController* own;
		CharacterController * other;
		++game->editor->ai1Num;
		if (game->editor->ai1Num < (AI_AMOUNT - 1))
		{
			own = ai1.own;
			other = ai1.other;
			ai1 = dojo.fighters[game->editor->ai1Num];
			ai1.own = own;
			own->controller = &ai1;
			ai1.other = other;

			own = ai2.own;
			other = ai2.other;
			game->editor->ai2Num = game->editor->ai1Num + 1;
			ai2 = dojo.fighters[game->editor->ai2Num];
			ai2.own = own;
			own->controller = &ai2;
			ai2.other = other;
		}
		else
		{
			dojo.generation = game->editor->generation;
			++game->editor->generation;
			game->editor->roundDuration += 10;
			std::sort(std::begin(dojo.fighters), std::end(dojo.fighters),
				[](const AI &a, const AI &b)
			{
				return a.fitness > b.fitness;
			});

			std::string n = "Brains/G" + std::to_string(game->editor->generation) + ".ai";
			Save(n);

			game->editor->fitness.push_back(dojo.fighters[0].fitness);

			NextGeneration(game->editor->mutationPosibiliy);
			for (unsigned i = 0u; i < AI_AMOUNT; ++i)
			{
				dojo.fighters[i].fitness = 0;
			}
			StartTrainning();
		}
	}
	game->characterController1->life = game->characterController1->lifeAmount;
	game->characterController1->pos = float3(game->editor->previewPos, 1.f);
	game->characterController1->state = CharacterController::CharacterStates::IDLE;
	game->characterController1->isGrounded = true;
	game->characterController2->life = game->characterController2->lifeAmount;
	game->characterController2->pos = float3(game->editor->previewPos2, 1.f);
	game->characterController2->state = CharacterController::CharacterStates::IDLE;
	game->characterController2->isGrounded = true;
}
