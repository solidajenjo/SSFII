#ifndef __SPRITE_H_
#define __SPRITE_H_

#include "ExternalLibraries/rapidjson-1.1.0/include/rapidjson/prettywriter.h"
#include "ExternalLibraries/rapidjson-1.1.0/include/rapidjson/document.h"
#include <string>

class Sprite
{
public:
	
	Sprite() {};
	Sprite(std::string sheetPath, unsigned x = 0u, unsigned y = 0u, unsigned w = 0u, unsigned h = 0u); 

	void CreateSprite();
	void Flush();
	void Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer) const;
	void UnSerialize(rapidjson::Value &value);
	//members

	std::string sheetPath;

	unsigned vbo = 0u, texture = 0u;
	int width = 0, height = 0;
	int x = 0, y = 0;
	float sW = 0.f, tW = 0.f;
};


#endif