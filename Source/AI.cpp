#include "AI.h"
#include "CharacterController.h"
#include <time.h>
#include "ExternalLibraries/imgui/imgui.h"
#include "ExternalLibraries/SDL/include/SDL_timer.h";
#include "Animation.h"
#include "Sprite.h"
#include "Game.h"
#include "FileSystem.h"

float AI::blockPrize = 20.f;
float AI::walkPrize = 1.f;
float AI::attackDistancePenalization = .005f;

AI::AI()
{
	Mutate(brain, 100u);
}

AI::~AI()
{
}

void AI::Reset()
{
	fitness = 0.f;
}

void AI::Mutate(Brain ancestor, unsigned mutationChance)
{
	for (unsigned j = 0u; j < OUTPUT_AMOUNT; ++j)
	{
		brain.neurons[j].value = 0.f;
		for (unsigned i = 0u; i < INPUT_AMOUNT; ++i)
		{
			if (rand() % 100 <= mutationChance)
			{
				brain.neurons[j].weights[i] = (rand() % 101) / 100.f;
			}
			else
			{
				brain.neurons[j].weights[i] = ancestor.neurons[j].weights[i];
			}
		}
	}

}

void AI::Save(std::string name, Brain b2, Brain b3, Brain b4, Brain b5)
{
	Brain brains[5] = { brain, b2, b3, b4, b5 };
	game->fileSystem->Write(name, &brains, sizeof(brains));
}

void AI::Load(std::string name)
{
	Brain brains[5];
	game->fileSystem->Read("Brains/" + name, &brains, sizeof(brains));
	brain = brains[0];
	game->aiS[1]->brain = brains[1];
	game->aiS[2]->brain = brains[2];
	game->aiS[3]->brain = brains[3];
	game->aiS[4]->brain = brains[4];
}

void AI::Update()
{
	float input[INPUT_AMOUNT];
	float distance = abs(own->pos.x - other->pos.x);

	for (unsigned i = 0u; i < (INPUT_AMOUNT - 1) / 2; ++i)
	{
		own->neuralNetworkInput.c_str()[i] == '0' ? input[i] = 0.f : input[i] = 1.f;
	}
	for (unsigned i = 0u; i < (INPUT_AMOUNT - 1) / 2; ++i)
	{
		other->neuralNetworkInput.c_str()[i] == '0' ? input[i + (INPUT_AMOUNT - 1) / 2] = 0.f : input[i + (INPUT_AMOUNT - 1) / 2] = 1.f;
	}
	input[INPUT_AMOUNT - 1] = distance / (float)SCREEN_WIDTH;

	for (unsigned j = 0u; j < OUTPUT_AMOUNT; ++j)	
	{
		brain.neurons[j].value = 0.f;
		for (unsigned i = 0u; i < INPUT_AMOUNT; ++i)
		{
			brain.neurons[j].value  += input[i] * brain.neurons[j].weights[i];
		}
		brain.neurons[j].value = 1.f / (1 + exp(-brain.neurons[j].value));
	}

	int op = 0;
	float max = brain.neurons[0].value;
	for (unsigned i = 1u; i < OUTPUT_AMOUNT; ++i)
	{
		if (brain.neurons[i].value > max)
		{
			max = brain.neurons[i].value;
			op = i;
		}
	}



	forward = false;
	backward = false;
	down = false;
	up = false;
	l_Punch = false;
	m_Punch = false;
	h_Punch = false;
	l_Kick = false;
	m_Kick = false;
	h_Kick = false;

	if (own->landingWaitTimer <= 0)
	{
		switch (op)
		{
		case 0:
			forward = true;
			break;
		case 1:
			backward = true;
			break;
		case 2:
			up = true;
			break;
		case 3:
			down = true;
			break;
		case 4:
			backward = true;
			up = true;
			break;
		case 5:
			forward = true;
			up = true;
			break;
		case 6:
			backward = true;
			down = true;
			break;
		case 7:
			forward = true;
			down = true;
			break;
		case 8:
			h_Punch = true;
			break;
		case 9:
			m_Punch = true;
			break;
		case 10:
			h_Punch = true;
			break;
		case 11:
			h_Kick = true;
			break;
		case 12:
			m_Kick = true;
			break;
		case 13:
			l_Kick = true;
			break;
		case 14:
			down = true;
			h_Kick = true;
			break;
		case 15:
			down = true;
			m_Kick = true;
			break;
		case 16:
			down = true;
			l_Kick = true;
			break;
		case 17:
			down = true;
			h_Punch = true;
			break;
		case 18:
			down = true;
			m_Punch = true;
			break;
		case 19:
			down = true;
			l_Punch = true;
			break;
		case 20:
			up = true;
			h_Kick = true;
			break;
		case 21:
			up = true;
			m_Kick = true;
			break;
		case 22:
			up = true;
			l_Kick = true;
			break;
		case 23:
			up = true;
			h_Punch = true;
			break;
		case 24:
			up = true;
			m_Punch = true;
			break;
		case 25:
			up = true;
			l_Punch = true;
			break;
		case 26:
			backward = true;
			up = true;
			h_Kick = true;
			break;
		case 27:
			backward = true;
			up = true;
			m_Kick = true;
			break;
		case 28:
			backward = true;
			up = true;
			l_Kick = true;
			break;
		case 29:
			backward = true;
			up = true;
			h_Punch = true;
			break;
		case 30:
			backward = true;
			up = true;
			m_Punch = true;
			break;
		case 31:
			backward = true;
			up = true;
			l_Punch = true;
			break;
		case 32:
			forward = true;
			up = true;
			h_Kick = true;
			break;
		case 33:
			forward = true;
			up = true;
			m_Kick = true;
			break;
		case 34:
			forward = true;
			up = true;
			l_Kick = true;
			break;
		case 35:
			forward = true;
			up = true;
			h_Punch = true;
			break;
		case 36:
			forward = true;
			up = true;
			m_Punch = true;
			break;
		case 37:
			forward = true;
			up = true;
			l_Punch = true;
			break;
		}
	}
	
	//fitness
	/*
	if (own->state == CharacterController::CharacterStates::WALK_FORWARD)
	{
		fitness += walkPrize * abs(own->pos.x - other->pos.x)
			- own->currentAnimation->frames[own->currentAnimation->currentFrame]->hitBoxes[1].box.Width() * 1.5f; //Walk forward when the other fighter is away is good stuff
	}
	if (own->state == CharacterController::CharacterStates::WALK_BACKWARDS && !other->isAttacking)
	{
		fitness -= walkPrize * abs(own->pos.x - other->pos.x); //Penalize fleeing fighters
	}
	if ((own->state == CharacterController::CharacterStates::BLOCK ||
		own->state == CharacterController::CharacterStates::CROUCH_BLOCK) && other->isAttacking)
	{
		float k = (1 - ((abs(own->pos.x - other->pos.x) / (float)SCREEN_WIDTH))) * blockPrize;
		fitness += k;
		LOG("BLOCK REWARD %.5f", k);
	}

	if (other->lastDamage > 0u) //hit is good
	{
		fitness += other->lastDamage * 3.f;
		other->lastDamage = 0u;
	}
	if (own->damageTaken > 0u) //get hit is bad :(
	{
		fitness -= own->damageTaken;
		own->damageTaken = 0u;
	}
	if (own->blocks > 0u) //block is sexy!
	{
		LOG("%s blocked %d attacks", name.c_str(), own->blocks);
		fitness += own->blocks * blockPrize;
		own->blocks = 0u;
	}

	if (own->isAttacking) //attack from far is for pussies
	{
		fitness -= abs(own->pos.x - other->pos.x) * attackDistancePenalization;
	}
	*/
}

bool AI::Forward(bool flipped) const
{
	return forward;
}

bool AI::Backward(bool flipped) const
{
	return backward;
}

bool AI::Down() const
{
	return down;
}

bool AI::Up() const
{
	return up;
}

bool AI::L_Punch() 
{
	if (l_Punch)
	{
		l_Punch = false;
		return true;
	}
	return false;
}

bool AI::M_Punch() 
{
	if (m_Punch)
	{
		m_Punch = false;
		return true;
	}
	return false;
}

bool AI::H_Punch() 
{
	if (h_Punch)
	{
		h_Punch = false;
		return true;
	}
	return false;
}

bool AI::L_Kick()
{
	if (l_Kick)
	{
		l_Kick = false;
		return true;
	}
	return false;
}

bool AI::M_Kick()
{
	if (m_Kick)
	{
		m_Kick = false;
		return true;
	}
	return false;
}

bool AI::H_Kick() 
{
	if (h_Kick)
	{
		h_Kick = false;
		return true;
	}
	return false;
}

void AI::Text()
{
	ImGui::PushID(this);
	ImGui::Text("Fitness %.3f", fitness);
	if (ImGui::TreeNodeEx("Decisions"))
	{
		for (unsigned i = 0u; i < OUTPUT_AMOUNT; ++i)
		{
			if (i % 5 != 0)
				ImGui::SameLine();
			ImGui::Text("%.5f", brain.neurons[i].value);
		}
		ImGui::TreePop();
	}
	ImGui::PopID();
}
