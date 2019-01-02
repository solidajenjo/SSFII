#ifndef __SPRITE_H_
#define __SPRITE_H_

class Sprite
{
public:
	
	Sprite(const char* sheetPath, float x, float y, float w, float h);

	unsigned vbo = 0, texture = 0;
	unsigned width = 0, height = 0;
};


#endif