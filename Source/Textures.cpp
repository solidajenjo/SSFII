#include "Game.h"
#include "Globals.h"
#include "ExternalLibraries/glew-2.1.0/include/GL/glew.h"
#include "ExternalLibraries/DevIL/include/IL/il.h"
#include "ExternalLibraries/DevIL/include/IL/ilu.h"
#include "ExternalLibraries/DevIL/include/IL/ilut.h"
#include "Textures.h"
#include "FileSystem.h"

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
		unsigned fileSize = game->fileSystem->Size(texturePath);
		if (fileSize > 0)
		{
			char* data = new char[fileSize];
			if (game->fileSystem->Read(texturePath, data, fileSize))
			{
				if (ilLoadL(IL_TYPE_UNKNOWN, data, fileSize))
				{
					ILinfo ImageInfo;
					iluGetImageInfo(&ImageInfo);
					if (ImageInfo.Origin == IL_ORIGIN_UPPER_LEFT)
					{
						iluFlipImage();
					}
					ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

					glGenTextures(1, &newTData->texId);
					glBindTexture(GL_TEXTURE_2D, newTData->texId);

					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

					w = ilGetInteger(IL_IMAGE_WIDTH);
					h = ilGetInteger(IL_IMAGE_HEIGHT);
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, ilGetData());

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
			else
			{
				LOG("Failed loading texture %s -> not found", texturePath);
				return 0;
			}
		}
		else
		{
			LOG("Failed loading texture %s -> not found", texturePath);
			return 0;
		}
	}
	w = (*it).second->w;
	h = (*it).second->h;
	++(*it).second->users;
	return (*it).second->texId;
}
