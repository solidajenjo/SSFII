#ifndef __AI_H_
#define __AI_H_

#include "Globals.h"
#include <string>
#include "ExternalLibraries/MathGeoLib/include/Math/float2.h"
#include "PlayerController.h"

#define INPUT_AMOUNT 51
#define OUTPUT_AMOUNT 40

class CharacterController;


struct Neuron
{
	float value = 0.f;
	float weights[INPUT_AMOUNT];
};

struct Brain
{	
	Neuron neurons[OUTPUT_AMOUNT];
};


class AI : public PlayerController
{
public:
		
	AI();
	~AI();

	void Reset();
	void Mutate(const Brain &ancestor, unsigned mutationChance); //mutate from ancestor
	void Cross(const Brain &b1, const Brain &b2); //cross two brains
	void Update();

	bool Forward(bool flipped) const override;
	bool Backward(bool flipped) const override;
	bool Down() const override;
	bool Up() const override;
	bool L_Punch() override;
	bool M_Punch() override;
	bool H_Punch() override;
	bool L_Kick() override;
	bool M_Kick() override;
	bool H_Kick() override;

	bool forward = false;
	bool backward = false;
	bool down = false;
	bool up = false;
	bool l_Punch = false;
	bool m_Punch = false;
	bool h_Punch = false;
	bool l_Kick = false;
	bool m_Kick = false;
	bool h_Kick = false;

	void Text();

	float fitness = 0.f;
	float dist = 0.f;
	float lastX = 0.f;
	CharacterController* own = nullptr;
	CharacterController* other = nullptr;

	Brain brain;

	static float blockPrize;
	static float walkPrize;
	static float attackDistancePenalization;

	char name[10] = "G0";
};


#endif