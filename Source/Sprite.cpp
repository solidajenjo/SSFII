#include "Game.h"
#include "Globals.h"
#include "ExternalLibraries/glew-2.1.0/include/GL/glew.h"
#include "ExternalLibraries/SDL_image/include/SDL_image.h"
#include "ExternalLibraries/MathGeoLib/include/Math/float3.h"
#include "Sprite.h"
#include "Textures.h"

//TODO: glDeleteBuffers(1, &vbo);

Sprite::Sprite(const char* sheetPath, float x, float y, float w, float h)
{
	width = w;
	height = h;
	int texW, texH;
	texture = game->textures->GetTexture(sheetPath, texW, texH);

	float s = x / (float)texW;
	float sW = w / (float)texW;
	float tW = h / (float)texH;
	float t = ((texH - y) / (float)texH) - tW;
	
	
	float vertex_buffer_data[] =
	{
		// positions
		-1, -1, 0.0f,
		1, -1, 0.0f,
		-1, 1, 0.0f,

		-1, 1, 0.0f,
		1, -1, 0.0f,
		1,  1, 0.0f,

		// uvs
		s, t,
		s + sW, t,
		s, t + tW,

		s, t + tW,
		s + sW, t,
		s + sW, t + tW
		
	};


	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data), vertex_buffer_data, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(
		0,                  // attribute 0
		3,                  // number of componentes (3 floats)
		GL_FLOAT,           // data type
		GL_FALSE,           // should be normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);
	glVertexAttribPointer(
		1,
		2,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)(sizeof(float) * 3 * 6)    // 3 float 6 vertices for jump positions
	);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
