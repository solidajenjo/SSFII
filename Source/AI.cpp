#include "AI.h"
#include "CharacterController.h"
#include <time.h>
#include "ExternalLibraries/imgui/imgui.h"
#include "ExternalLibraries/SDL/include/SDL_timer.h";
#include "Animation.h"
#include "Sprite.h"
#include "Game.h"
#include "FileSystem.h"

float AI::blockPrize = 500.f;
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
	lastX = own->pos.x;
}

void AI::Mutate(const Brain & ancestor, unsigned mutationChance)
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

void AI::Cross(const Brain & b1, const Brain & b2)
{
	for (unsigned j = 0u; j < OUTPUT_AMOUNT; ++j)
	{
		brain.neurons[j].value = 0.f;
		for (unsigned i = 0u; i < INPUT_AMOUNT; ++i)
		{
			if (rand() % 100 <= 50)
			{
				brain.neurons[j].weights[i] = b1.neurons[j].weights[i];
			}
			else
			{
				brain.neurons[j].weights[i] = b2.neurons[j].weights[i];
			}
		}
	}
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

	up = false;
	l_Punch = false;
	m_Punch = false;
	h_Punch = false;
	l_Kick = false;
	m_Kick = false;
	h_Kick = false;

	bool cond1 = own->landingWaitTimer <= 0 && !((own->state == CharacterController::CharacterStates::BLOCK && other->isAttacking && !other->isGrounded)) && !((own->state == CharacterController::CharacterStates::CROUCH_BLOCK && other->isAttacking && other->isGrounded));
	bool cond2 = (own->state == CharacterController::CharacterStates::CROUCH_BLOCK && other->isAttacking && !other->isGrounded);
	if (cond1 || cond2)
	{
		forward = false;
		backward = false;
		down = false;

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
	float deltaX = abs(own->pos.x - lastX);
	lastX = own->pos.x;

	if (own->state == CharacterController::CharacterStates::WALK_FORWARD)
	{
		fitness += walkPrize * deltaX * 0.000001f * (distance * distance);
	}
	if (own->state == CharacterController::CharacterStates::WALK_BACKWARDS && !other->isAttacking)
	{
		fitness -= walkPrize * 0.000001f * (distance * distance); //Penalize fleeing fighters
	}
	if ((own->state == CharacterController::CharacterStates::BLOCK ||
		own->state == CharacterController::CharacterStates::CROUCH_BLOCK) && other->isAttacking)
	{
		float k = blockPrize / distance;
		k = distance < 300.f ? k : 0.f;
		fitness += k;
		LOG("BLOCK REWARD %.5f DIST %.5f", k, distance);
	}

	if (other->lastDamage > 0u) //hit is good
	{
		fitness += other->lastDamage * 10.f;
		other->lastDamage = 0u;
	}
	if (own->damageTaken > 0u) //get hit is bad :(
	{
		fitness -= own->damageTaken;
		own->damageTaken = 0u;
	}

	/*
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
	switch (own->state) {
	case CharacterController::CharacterStates::IDLE:
		ImGui::Text("STATE: IDLE");
		break;
	case CharacterController::CharacterStates::WALK_FORWARD:
		ImGui::Text("STATE: WALK_FORWARD");
		break;
	case CharacterController::CharacterStates::WALK_BACKWARDS:
		ImGui::Text("STATE: WALK_BACKWARDS");
		break;
	case CharacterController::CharacterStates::ATTACK:
		ImGui::Text("STATE: ATTACK");
		break;
	case CharacterController::CharacterStates::BLOCK:
		ImGui::Text("STATE: BLOCK");
		break;
	case CharacterController::CharacterStates::CROUCH:
		ImGui::Text("STATE: CROUCH");
		break;
	case CharacterController::CharacterStates::CROUCH_ATTACK:
		ImGui::Text("STATE: CROUCH_ATTACK");
		break;
	case CharacterController::CharacterStates::CROUCH_BLOCK:
		ImGui::Text("STATE: CROUCH_BLOCK");
		break;
	case CharacterController::CharacterStates::FORWARD_ATTACK:
		ImGui::Text("STATE: FORWARD_ATTACK");
		break;
	case CharacterController::CharacterStates::BACKWARDS_ATTACK:
		ImGui::Text("STATE: BACKWARDS_ATTACK");
		break;
	case CharacterController::CharacterStates::JUMP:
		ImGui::Text("STATE: JUMP");
		break;
	case CharacterController::CharacterStates::JUMP_ATTACK:
		ImGui::Text("STATE: JUMP_ATTACK");
		break;
	case CharacterController::CharacterStates::JUMP_FORWARD:
		ImGui::Text("STATE: JUMP_FORWARD");
		break;
	case CharacterController::CharacterStates::JUMP_FORWARD_ATTACK:
		ImGui::Text("STATE: JUMP_FORWARD_ATTACK");
		break;
	case CharacterController::CharacterStates::JUMP_BACKWARDS:
		ImGui::Text("STATE: JUMP_BACKWARDS");
		break;
	case CharacterController::CharacterStates::JUMP_BACKWARDS_ATTACK:
		ImGui::Text("STATE: JUMP_BACKWARDS_ATTACK");
		break;
	case CharacterController::CharacterStates::FACE_HIT:
		ImGui::Text("STATE: FACE_HIT");
		break;
	case CharacterController::CharacterStates::BODY_HIT:
		ImGui::Text("STATE: BODY_HIT");
		break;

		
	};

	ImGui::PopID();
}
