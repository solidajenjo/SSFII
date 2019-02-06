#include "Fx.h"
#include "Animation.h"
#include "Game.h"
#include "Globals.h"
#include "FileSystem.h"

Fx::Fx(std::string sheetName) : sheetName(sheetName)
{
	animation = new Animation(0);
}

Fx::~Fx()
{
	RELEASE(animation);
}

void Fx::Serialize() const
{
	rapidjson::StringBuffer sb;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);
	writer.StartArray();
	writer.StartObject();
	writer.String("spriteSheetPath"); writer.String(sheetPath.c_str());
	writer.String("animation");
	animation->Serialize(writer);
	writer.EndObject();
	if (game->fileSystem->Write("AnimSheets/" + sheetName, sb.GetString(), strlen(sb.GetString())))
	{
		LOG("Fx saved.");
	}
}

bool Fx::LoadSheet()
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
			animation = new Animation(0);
			animation->UnSerialize(sheetJSON[0]["animation"]);						
		}
	}
	else
		return false;

	return true;
}
