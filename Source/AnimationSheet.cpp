#include "AnimationSheet.h"
#include "Animation.h"
#include "Game.h"
#include "FileSystem.h"

void AnimationSheet::Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer) const
{
	rapidjson::StringBuffer sb;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);
	writer.StartArray();
	for (unsigned i = 0u; i < ANIM_NUM; ++i)
	{
		animations[i]->Serialize(writer);
	}
	writer.EndArray();
	if (game->fileSystem->Write(sheetPath, sb.GetString(), strlen(sb.GetString()), true))
	{
		LOG("Scene saved.");
	}
}
