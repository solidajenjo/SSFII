#ifndef __FX_H_
#define __FX_H_

#include "Globals.h"
#include <string>
#include "ExternalLibraries/rapidjson-1.1.0/include/rapidjson/prettywriter.h"


class Animation;

class Fx
{
public:
		
	Fx(std::string sheetName);
	~Fx();

	void Serialize() const;
	bool LoadSheet();

	//members
	
	std::string sheetName = "";
	std::string sheetPath = "";
	
	Animation* animation = nullptr;

};


#endif