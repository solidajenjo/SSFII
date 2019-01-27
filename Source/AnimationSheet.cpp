#include "AnimationSheet.h"
#include "Animation.h"
#include "Game.h"
#include "Globals.h"
#include "FileSystem.h"

AnimationSheet::AnimationSheet(std::string sheetName) : sheetName(sheetName)
{
	animations = new Animation*[ANIM_NUM];
	for (unsigned i = 0u; i < ANIM_NUM; ++i)
	{
		animations[i] = new Animation(0);
		animations[i]->name = ANIM_NAMES[i];
	}
}

AnimationSheet::~AnimationSheet()
{
	for (unsigned i = 0u; i < ANIM_NUM; ++i)
		RELEASE(animations[i]);
	RELEASE_ARRAY(animations);
}

void AnimationSheet::Serialize() const
{
	rapidjson::StringBuffer sb;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);
	writer.StartArray();
	writer.StartObject();
	writer.String("spriteSheetPath"); writer.String(sheetPath.c_str());
	writer.EndObject();
	for (unsigned i = 0u; i < ANIM_NUM; ++i)
	{
		animations[i]->Serialize(writer);
	}
	writer.EndArray();
	if (game->fileSystem->Write("AnimSheets/" + sheetName, sb.GetString(), strlen(sb.GetString())))
	{
		LOG("AnimationSheet saved.");
	}
}

bool AnimationSheet::LoadSheet()
{
	unsigned fileSize = game->fileSystem->Size("AnimSheets/" + sheetName);
	char* buffer = new char[fileSize];
	if (game->fileSystem->Read("AnimSheets/" + sheetName, buffer, fileSize))
	{
		rapidjson::Document document;
		if (document.Parse<rapidjson::kParseStopWhenDoneFlag>(buffer).HasParseError())
		{
			LOG("Error loading sheet %s. Sheet file corrupted.", sheetName.c_str());
			auto error = rapidjson::GetParseErrorFunc(document.GetParseError());
			return false;
		}
		else
		{
			rapidjson::Value sheetJSON = document.GetArray();
			sheetPath = sheetJSON[0]["spriteSheetPath"].GetString();
			for (unsigned i = 1u; i <= ANIM_NUM; ++i)
			{
				animations[i - 1]->UnSerialize(sheetJSON[i]);
			}
		}
	}
	else
		return false;

	return true;
}
