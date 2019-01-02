#ifndef __TEXTURES_H_
#define __TEXTURES_H_

#include <map>
#include <string>

class Textures
{
public:
	
	struct TextureData
	{
		unsigned texId = 0;
		unsigned users = 0;
		unsigned w = 0;
		unsigned h = 0;
	};

	void Init();
	unsigned GetTexture(const char* texturePath, int &w, int &h);

private:

	std::map<std::string, TextureData*> texturesContainer;
};


#endif