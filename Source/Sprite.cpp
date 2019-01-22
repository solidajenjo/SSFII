#include "Game.h"
#include "Globals.h"
#include "ExternalLibraries/glew-2.1.0/include/GL/glew.h"
#include "ExternalLibraries/SDL_image/include/SDL_image.h"
#include "ExternalLibraries/MathGeoLib/include/Math/float3.h"
#include "Sprite.h"
#include "Textures.h"

//TODO: glDeleteBuffers(1, &vbo);

Sprite::Sprite(std::string sheetPath, unsigned x, unsigned y, unsigned w, unsigned h) : width(w), height(h), x(x), y(y), sheetPath(sheetPath)
{
	int texW, texH;
	texture = game->textures->GetTexture(sheetPath.c_str(), texW, texH);

	float s = x / (float)texW;
	float sW = w / (float)texW;
	float tW = h / (float)texH;
	float t = ((texH - y) / (float)texH) - tW;
	
	
	float vertex_buffer_data[] =
	{
		// positions
		-1.f, .0f, .0f,
		1.f, .0f, .0f,
		-1.f, 2.f, .0f,

		-1.f, 2.f, .0f,
		1.f, .0f, .0f,
		1.f,  2.f, .0f,

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

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Sprite::Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer) const
{
	writer.StartObject();
	writer.String("sheetPath"); writer.String(sheetPath.c_str());
	writer.String("width"); writer.Int(width);
	writer.String("height"); writer.Int(height);
	writer.String("x"); writer.Int(x);
	writer.String("y"); writer.Int(y);
	writer.EndObject();
}