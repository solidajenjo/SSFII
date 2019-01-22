#ifndef __SPRITE_H_
#define __SPRITE_H_

#include "ExternalLibraries/rapidjson-1.1.0/include/rapidjson/prettywriter.h"
#include "ExternalLibraries/rapidjson-1.1.0/include/rapidjson/document.h"
#include <string>

class Sprite
{
public:
	
	Sprite() {};
	Sprite(std::string sheetPath, unsigned x, unsigned y, unsigned w, unsigned h);

	void CreateSprite();
	void Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer) const;
	void UnSerialize(rapidjson::Value &value);
	//members

	std::string sheetPath;

	unsigned vbo = 0u, texture = 0u;
	unsigned width = 0u, height = 0u;
	unsigned x = 0u, y = 0u;
};


#endif