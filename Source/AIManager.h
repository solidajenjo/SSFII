#ifndef __AI_MANAGER_H_
#define __AI_MANAGER_H_

#define AI_AMOUNT 30

#include "Globals.h"
#include <string>
#include "AI.h"

class AIManager
{

	struct AIDojo
	{
		AI fighters[AI_AMOUNT];
		unsigned generation = 0u;
	};

public:

	void Save(std::string &name) const;
	void Load(std::string &name);
	void NextGeneration(unsigned mutationChance);
	void StartTrainning();
	void Train();


	AIDojo dojo;

	AI ai1;
	AI ai2;
};

#endif