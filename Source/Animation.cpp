#include "Game.h"
#include "Globals.h"
#include "Render.h"
#include "Animation.h"
#include "Sprite.h"
#include "ExternalLibraries/SDL/include/SDL_timer.h"
#include "ExternalLibraries/glew-2.1.0/include/GL/glew.h"


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
	game->render->RenderSprite(frames[currentFrame]->sprite, pos + float3(frames[currentFrame]->offsetH, frames[currentFrame]->offsetV, .0f));

}

void Animation::Reset(unsigned newNFrames, std::string sheetPath)
{
	for (unsigned i = 0u; i < nFrames; ++i)
		RELEASE(frames[i]);
	RELEASE_ARRAY(frames);
	nFrames = newNFrames;
	frames = new Frame*[nFrames];
	for (unsigned i = 0u; i < nFrames; ++i)
	{
		frames[i] = new Frame();
		if (sheetPath != "")
			frames[i]->sprite = new Sprite(sheetPath);
	}
}

void Animation::Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer) const
{
	writer.StartObject();
	writer.String("nFrames"); writer.Int(nFrames);
	writer.String("frameDuration"); writer.Double(frameDuration); 
	if (nFrames > 0)
	{
		writer.String("frames");
		writer.StartArray();
		for (unsigned i = 0u; i < nFrames; ++i)
		{		
			writer.StartObject();
			writer.String("sprite");
			frames[i]->sprite->Serialize(writer);
			writer.String("hitboxes");
			writer.StartArray();
			for (unsigned j = 0u; j < 4u; ++j)
			{
				writer.StartObject();				
				writer.String("x"); writer.Double(frames[i]->hitBoxes[j].box.minPoint.x);
				writer.String("y"); writer.Double(frames[i]->hitBoxes[j].box.minPoint.y);
				writer.String("X"); writer.Double(frames[i]->hitBoxes[j].box.maxPoint.x);
				writer.String("Y"); writer.Double(frames[i]->hitBoxes[j].box.maxPoint.y);
				writer.String("damage"); writer.Int(frames[i]->hitBoxes[j].damageAmount);
				writer.EndObject();
			}
			writer.EndArray();
			writer.String("offsetH"); writer.Int(frames[i]->offsetH);
			writer.String("offsetV"); writer.Int(frames[i]->offsetV);
			writer.EndObject();
		}
		writer.EndArray();
	}
	writer.EndObject();
}

void Animation::UnSerialize(rapidjson::Value & value)
{
	int newNFrames = value["nFrames"].GetInt();
	frameDuration = value["frameDuration"].GetDouble();
	Reset(newNFrames, "");
	for (unsigned i = 0u; i < nFrames; ++i)
	{
		frames[i]->sprite = new Sprite();
		frames[i]->sprite->UnSerialize(value["frames"][i]["sprite"]);	
		for (unsigned j = 0u; j < 4u; ++j)
		{
			frames[i]->hitBoxes[j].box.minPoint.x = value["frames"][i]["hitboxes"][j]["x"].GetDouble();
			frames[i]->hitBoxes[j].box.minPoint.y = value["frames"][i]["hitboxes"][j]["y"].GetDouble();
			frames[i]->hitBoxes[j].box.maxPoint.x = value["frames"][i]["hitboxes"][j]["X"].GetDouble();
			frames[i]->hitBoxes[j].box.maxPoint.y = value["frames"][i]["hitboxes"][j]["Y"].GetDouble();
			frames[i]->hitBoxes[j].damageAmount = value["frames"][i]["hitboxes"][j]["damage"].GetInt();
		}
		frames[i]->offsetH = value["frames"][i]["offsetH"].GetInt();
		frames[i]->offsetV = value["frames"][i]["offsetV"].GetInt();
	}
}

