#ifndef __ANIMATIONSHEET_H_
#define __ANIMATIONSHEET_H_

#include "Globals.h"
#include <string>
#include <vector>

class Animation;

class AnimationSheet
{
public:
	
	enum class Animations //TDODO: Add grab possibility?
	{
		IDLE, WALK, JUMP, FORWARD_JUMP, CROUCH, BLOCKING, BLOCKING_CROUCH,
		L_PUNCH, M_PUNCH, H_PUNCH, F_L_PUNCH, F_M_PUNCH, F_J_PUNCH, L_KICK, M_KICK, H_KICK,
		F_L_KICK, F_M_KICK, F_H_KICK, C_L_PUNCH, C_M_PUNCH, C_H_PUNCH, C_L_KICK, C_M_KICK,
		C_H_KICK, J_PUNCH, J_KICK, SPECIAL_1, SPECIAL_2, HIT, FACE_HIT, CROUCH_HIT, KNOCK_DOWN,
		VICTORY
	};

	AnimationSheet(std::string sheetName, std::string sheetPath, std::string sheetSource) : sheetName(sheetName), sheetPath(sheetPath), sheetSource(sheetSource) {};

	//members
	
	std::string sheetName = "";
	std::string sheetPath = "";
	std::string sheetSource = "";

	std::vector<Animation*> animations = std::vector<Animation*>(ANIM_NUM);

	std::string ANIM_NAMES[ANIM_NUM] = { "IDLE","WALK","JUMP","FORWARD_JUMP","CROUCH","BLOCKING","BLOCKING_CROUCH",
		"L_PUNCH","M_PUNCH","H_PUNCH","F_L_PUNCH","F_M_PUNCH","F_J_PUNCH","L_KICK","M_KICK","H_KICK",
		"F_L_KICK","F_M_KICK","F_H_KICK","C_L_PUNCH","C_M_PUNCH","C_H_PUNCH","C_L_KICK","C_M_KICK",
		"C_H_KICK","J_PUNCH","J_KICK","SPECIAL_1","SPECIAL_2","HIT","FACE_HIT","CROUCH_HIT","KNOCK_DOWN",
		"VICTORY" };
};


#endif