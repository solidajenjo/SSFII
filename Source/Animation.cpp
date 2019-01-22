#include "Game.h"
#include "Globals.h"
#include "Render.h"
#include "Animation.h"
#include "Sprite.h"
#include "ExternalLibraries/SDL/include/SDL_timer.h"


Animation::Animation(unsigned nFrames) : nFrames(nFrames)
{
	frames = new Frame*[nFrames];
	for (unsigned i = 0u; i < nFrames; ++i)
		frames[i] = new Frame();
}

Animation::~Animation()
{
	for (unsigned i = 0u; i < nFrames; ++i)
		RELEASE(frames[i]);
	RELEASE_ARRAY(frames);
}

bool Animation::SetFrame(unsigned frameNum, Sprite* sprite)
{
	if (frameNum >= nFrames)
	{
		LOG("Frame number out of range.");
	}
	frames[frameNum]->sprite = sprite;
	return false;
}

void Animation::Play(const float3 &pos, bool &loopEnded)
{
	lastTime = SDL_GetTicks();
	if (lastTime >= nextFrameChange)
	{
		nextFrameChange = lastTime + frameDuration;
		++currentFrame;
		if (currentFrame >= nFrames)
		{
			currentFrame = 0u;
			loopEnded = true;
		}
	}
	game->render->RenderSprite(frames[currentFrame]->sprite, pos);
}

void Animation::Reset()
{
	for (unsigned i = 0u; i < nFrames; ++i)
		RELEASE(frames[i]);
	RELEASE_ARRAY(frames);
	frames = new Frame*[nFrames];
	for (unsigned i = 0u; i < nFrames; ++i)
		frames[i] = new Frame();
}

void Animation::Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer) const
{
	writer.StartObject();
	writer.String("nFrames"); writer.Int(nFrames);
	writer.String("frameDuration"); writer.Double(frameDuration);
	writer.StartArray();
	for (unsigned i = 0u; i < nFrames; ++i)
	{
		writer.StartObject();
		writer.String("sprite");
		frames[i]->sprite->Serialize(writer);
		writer.String("hitBoxes");
		writer.StartArray();
		for (unsigned j = 0u; j < 4u; ++j)
		{
			writer.StartObject();
			writer.String("doDamage"); writer.Bool(frames[i]->hitBoxes[j].doDamage);
			writer.String("maxPoint X"); writer.Double(frames[i]->hitBoxes[j].box->maxPoint.x);
			writer.String("minPoint X"); writer.Double(frames[i]->hitBoxes[j].box->minPoint.x);
			writer.String("maxPoint Y"); writer.Double(frames[i]->hitBoxes[j].box->maxPoint.y);
			writer.String("minPoint Y"); writer.Double(frames[i]->hitBoxes[j].box->minPoint.y);
			writer.EndObject();
		}
		writer.EndArray();
		writer.EndObject();
	}
	writer.EndArray();
	writer.EndObject();
}

void Animation::UnSerialize(rapidjson::Value & value)
{
	nFrames = value["nFrames"].GetInt();
	frameDuration = value["frameDuration"].GetDouble();
	Reset();
	for (unsigned i = 0u; i < nFrames; ++i)
	{
		frames[i]->sprite = new Sprite();
		frames[i]->sprite->UnSerialize(value[i]["sprite"]);
		for (unsigned j = 0u; j < 4u; ++j)
		{
			frames[i]->hitBoxes[j].doDamage = value[i]["hitBoxes"][j]["doDamage"].GetBool();
			frames[i]->hitBoxes[j].box->minPoint = float2(value[i]["hitBoxes"][j]["minPoint X"].GetDouble(),
				value[i]["hitBoxes"][j]["minPoint Y"].GetDouble());
			frames[i]->hitBoxes[j].box->maxPoint = float2(value[i]["hitBoxes"][j]["maxPoint X"].GetDouble(),
				value[i]["hitBoxes"][j]["maxPoint Y"].GetDouble());
		}
	}
}

