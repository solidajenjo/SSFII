#include "AI.h"
#include "CharacterController.h"
#include <time.h>
#include "ExternalLibraries/imgui/imgui.h"
#include "ExternalLibraries/SDL/include/SDL_timer.h"

void InputLayer::FeedInput(const std::string & ownCharacter, const std::string & otherCharacter, const int distance)
{
	for (unsigned i = 0u; i < (INPUT_AMOUNT - 1) / 2; ++i)
	{
		ownCharacter.c_str()[i] == '0' ? input[i] = 0.f : input[i] = 1.f;
	}
	for (unsigned i = 0u; i < (INPUT_AMOUNT - 1) / 2; ++i)
	{
		otherCharacter.c_str()[i] == '0' ? input[i + (INPUT_AMOUNT - 1) / 2] = 0.f : input[i + (INPUT_AMOUNT - 1) / 2] = 1.f;
	}
	input[INPUT_AMOUNT - 1] = abs(distance) / SCREEN_WIDTH;
}

Neuron::Neuron()
{		
	for (unsigned i = 0u; i < INPUT_AMOUNT; ++i)
	{		
		weights[i] = rand() % 101 / 100.f;
	}
}

void Neuron::Calculate(const float* input)
{
	float x = 0.f;
	for (unsigned i = 0u; i < INPUT_AMOUNT; ++i)
	{
		x += input[i] * weights[i];
	}
	value = 1.f /(1.f + exp(-x));
	if (value < 0.9f)
		value = 0.f;
}

void Neuron::Calculate()
{
	float x = 0.f;
	for (unsigned i = 0u; i < INPUT_AMOUNT; ++i)
	{
		x += inputs[i] * weights[i];
	}
	value = 1.f / (1.f + exp(-x));
	value = x;
}

Layer::Layer(int size) : size(size)
{
	neurons = new Neuron*[size];
	for (unsigned i = 0u; i < size; ++i)
	{
		neurons[i] = new Neuron();
	}
}

Layer::~Layer()
{
	for (unsigned i = 0u; i < size; ++i)
	{
		RELEASE(neurons[i]);
	}
	RELEASE_ARRAY(neurons);
}

void Layer::FeedLayer(const InputLayer &il)
{
	for (unsigned i = 0u; i < size; ++i)
	{
		neurons[i]->Calculate(il.input);
	}
}

void Layer::FeedLayer(const Layer* sourceLayer)
{
	for (unsigned i = 0u; i < sourceLayer->size; ++i)
		for (unsigned j = 0u; j < size; ++j)
		{
			neurons[j]->inputs[i] = sourceLayer->neurons[i]->value;
		}
	for (unsigned j = 0u; j < size; ++j)
	{
		neurons[j]->Calculate();
	}
}

AI::AI()
{
	hiddenLayer = new Layer(INPUT_AMOUNT);
	outputLayer = new Layer(OUTPUT_AMOUNT);
}

AI::~AI()
{
	RELEASE(hiddenLayer);
	RELEASE(outputLayer);
}

void AI::Reset()
{
	fitness = 0.f;
}

void AI::Update()
{
	il.FeedInput(own->neuralNetworkInput, other->neuralNetworkInput, own->pos.x - other->pos.x);	
	hiddenLayer->FeedLayer(il);
	outputLayer->FeedLayer(hiddenLayer);
	int op = 0;
	float max = outputLayer->neurons[0]->value;
	for (unsigned i = 1u; i < outputLayer->size; ++i)
	{
		if (outputLayer->neurons[i]->value > max)
		{
			max = outputLayer->neurons[i]->value;
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
	
	//fitness
		
	if (other->lastDamage > 0u)
	{
		fitness += other->lastDamage * 10u;
		other->lastDamage = 0u;
	}
	if (own->damageTaken > 0u)
	{
		fitness -= own->damageTaken * 10u;
		own->damageTaken = 0u;
	}
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
	ImGui::Text("Fitness %.3f", fitness);
	for (unsigned i = 0u; i < outputLayer->size; ++i)
	{
		if (i % 5 != 0)
			ImGui::SameLine();
		ImGui::Text("%.5f", outputLayer->neurons[i]->value);
	}
}
