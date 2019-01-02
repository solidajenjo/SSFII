#include "Game.h"
#include "Globals.h"
#include "ExternalLibraries/glew-2.1.0/include/GL/glew.h"
#include "ExternalLibraries/DevIL/include/IL/il.h"
#include "ExternalLibraries/DevIL/include/IL/ilu.h"
#include "ExternalLibraries/DevIL/include/IL/ilut.h"
#include "Textures.h"

void Textures::Init()
{
	ilInit();
	iluInit();
	ilutInit();
}
unsigned Textures::GetTexture(const char* texturePath, int &w, int &h)
{
	std::map<std::string, TextureData*>::iterator it = texturesContainer.find(texturePath);
	if (it == texturesContainer.end())
	{
		TextureData* newTData = new TextureData();
		ILuint ImageName;
		ilGenImages(1, &ImageName);

		ilBindImage(ImageName);
		ILboolean succes = ilLoadImage(texturePath);
		if (succes)
		{
			ILinfo ImageInfo;
			iluGetImageInfo(&ImageInfo);
			if (ImageInfo.Origin == IL_ORIGIN_UPPER_LEFT)
			{
				iluFlipImage();
			}
			ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE);

			glGenTextures(1, &newTData->texId);
			glBindTexture(GL_TEXTURE_2D, newTData->texId);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

			w = ilGetInteger(IL_IMAGE_WIDTH);	
			h = ilGetInteger(IL_IMAGE_HEIGHT);	
			glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_FORMAT), w, h,	0, ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE, ilGetData());

			newTData->w = w;
			newTData->h = h;
			++newTData->users;

			texturesContainer[texturePath] = newTData;
			return newTData->texId;
		}
		else 
		{
			LOG("Failed loading texture %s -> %s", texturePath, iluErrorString(ilGetError()));
			return 0;
		}
	}
	w = (*it).second->w;
	h = (*it).second->h;
	++(*it).second->users;
	return (*it).second->texId;
}
