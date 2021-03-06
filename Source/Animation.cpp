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

void Animation::Play(const float3 &pos, bool &loopEnded, bool flip, float3 color, bool loop)
{
	lastTime = SDL_GetTicks();
	loopEnded = false;
	if (lastTime >= nextFrameChange)
	{
		if (loop)
		{
			nextFrameChange = lastTime + frameDuration;
			if (!reverse)
			{
				++currentFrame;
				if (currentFrame >= nFrames)
				{
					currentFrame = 0u;
					loopEnded = true;
				}
			}
			else
			{
				if (currentFrame == 0)
				{
					currentFrame = nFrames - 1;
					loopEnded = true;
				}
				else
					--currentFrame;
			}
		}
		else
		{
			nextFrameChange = lastTime + frameDuration;
			if (!reverse)
			{
				++currentFrame;
				if (currentFrame >= nFrames)
				{
					currentFrame = nFrames - 1;
					loopEnded = true;
				}
			}
			else
			{
				if (currentFrame == 0)
				{
					loopEnded = true;
				}
				else
					--currentFrame;
			}
		}
	}
	game->render->RenderSprite(frames[currentFrame]->sprite, pos, scale, frames[currentFrame]->offsetH, frames[currentFrame]->offsetV, flip, color);
	UpdateHBoxes(pos.xy(), flip);
	DrawHBoxes();
}

void Animation::UpdateHBoxes(const float2 & pos, bool flip)
{
	if (!flip)
	{
		if (frames[currentFrame]->hitBoxes[0].box.minPoint.Equals(frames[currentFrame]->hitBoxes[0].box.maxPoint))
		{
			hitBoxes[0].enabled = false;
			hitBoxes[0].box.SetNegativeInfinity();
		}
		else
		{
			hitBoxes[0].box.minPoint = float2(pos.x + frames[currentFrame]->offsetH + frames[currentFrame]->hitBoxes[0].box.minPoint.x * scale,
				pos.y + frames[currentFrame]->hitBoxes[0].box.minPoint.y * scale);
			hitBoxes[0].box.maxPoint = float2(pos.x + frames[currentFrame]->offsetH + frames[currentFrame]->hitBoxes[0].box.maxPoint.x * scale,
				pos.y + frames[currentFrame]->hitBoxes[0].box.maxPoint.y * scale);
			hitBoxes[0].enabled = true;
		}
		if (frames[currentFrame]->hitBoxes[1].box.minPoint.Equals(frames[currentFrame]->hitBoxes[1].box.maxPoint))
		{
			hitBoxes[1].enabled = false;
			hitBoxes[1].box.SetNegativeInfinity();
		}
		else
		{
			hitBoxes[1].box.minPoint = float2(pos.x + frames[currentFrame]->offsetH + frames[currentFrame]->hitBoxes[1].box.minPoint.x * scale,
				pos.y + frames[currentFrame]->hitBoxes[1].box.minPoint.y * scale);
			hitBoxes[1].box.maxPoint = float2(pos.x + frames[currentFrame]->offsetH + frames[currentFrame]->hitBoxes[1].box.maxPoint.x * scale,
				pos.y + frames[currentFrame]->hitBoxes[1].box.maxPoint.y * scale);
			hitBoxes[1].enabled = true;
		}
		if (frames[currentFrame]->hitBoxes[2].box.minPoint.Equals(frames[currentFrame]->hitBoxes[2].box.maxPoint))
		{
			hitBoxes[2].enabled = false;
			hitBoxes[2].box.SetNegativeInfinity();
		}
		else
		{
			hitBoxes[2].box.minPoint = float2(pos.x + frames[currentFrame]->offsetH + frames[currentFrame]->hitBoxes[2].box.minPoint.x * scale,
				pos.y + frames[currentFrame]->hitBoxes[2].box.minPoint.y * scale);
			hitBoxes[2].box.maxPoint = float2(pos.x + frames[currentFrame]->offsetH + frames[currentFrame]->hitBoxes[2].box.maxPoint.x * scale,
				pos.y + frames[currentFrame]->hitBoxes[2].box.maxPoint.y * scale);
			hitBoxes[2].enabled = true;
		}
	}
	else																							 
	{			
		if (frames[currentFrame]->hitBoxes[0].box.minPoint.Equals(frames[currentFrame]->hitBoxes[0].box.maxPoint))
		{
			hitBoxes[0].enabled = false;
			hitBoxes[0].box.SetNegativeInfinity();
		}
		else
		{
			hitBoxes[0].box.minPoint = float2(pos.x - frames[currentFrame]->offsetH - frames[currentFrame]->hitBoxes[0].box.maxPoint.x * scale,
				pos.y + frames[currentFrame]->offsetV + frames[currentFrame]->hitBoxes[0].box.minPoint.y * scale);
			hitBoxes[0].box.maxPoint = float2(pos.x - frames[currentFrame]->offsetH - frames[currentFrame]->hitBoxes[0].box.minPoint.x * scale,
				pos.y + frames[currentFrame]->offsetV + frames[currentFrame]->hitBoxes[0].box.maxPoint.y * scale);
			hitBoxes[0].enabled = true;
		}
		if (frames[currentFrame]->hitBoxes[1].box.minPoint.Equals(frames[currentFrame]->hitBoxes[1].box.maxPoint))
		{
			hitBoxes[1].enabled = false;
			hitBoxes[1].box.SetNegativeInfinity();
		}
		else
		{
			hitBoxes[1].box.minPoint = float2(pos.x - frames[currentFrame]->offsetH - frames[currentFrame]->hitBoxes[1].box.maxPoint.x * scale,
				pos.y + frames[currentFrame]->offsetV + frames[currentFrame]->hitBoxes[1].box.minPoint.y * scale);
			hitBoxes[1].box.maxPoint = float2(pos.x - frames[currentFrame]->offsetH - frames[currentFrame]->hitBoxes[1].box.minPoint.x * scale,
				pos.y + frames[currentFrame]->offsetV + frames[currentFrame]->hitBoxes[1].box.maxPoint.y * scale);
			hitBoxes[1].enabled = true;
		}
		if (frames[currentFrame]->hitBoxes[2].box.minPoint.Equals(frames[currentFrame]->hitBoxes[2].box.maxPoint))
		{
			hitBoxes[2].enabled = false;
			hitBoxes[2].box.SetNegativeInfinity();
		}
		else
		{
			hitBoxes[2].box.minPoint = float2(pos.x - frames[currentFrame]->offsetH - frames[currentFrame]->hitBoxes[2].box.maxPoint.x * scale,
				pos.y + frames[currentFrame]->offsetV + frames[currentFrame]->hitBoxes[2].box.minPoint.y * scale);
			hitBoxes[2].box.maxPoint = float2(pos.x - frames[currentFrame]->offsetH - frames[currentFrame]->hitBoxes[2].box.minPoint.x * scale,
				pos.y + frames[currentFrame]->offsetV + frames[currentFrame]->hitBoxes[2].box.maxPoint.y * scale);
			hitBoxes[2].enabled = true;
		}
	}
}

void Animation::DrawHBoxes() const
{
	game->render->DrawBox(hitBoxes[0].box.minPoint, hitBoxes[0].box.maxPoint);
	game->render->DrawBox(hitBoxes[1].box.minPoint, hitBoxes[1].box.maxPoint);
	game->render->DrawBox(hitBoxes[2].box.minPoint, hitBoxes[2].box.maxPoint, true);
}

void Animation::Rewind()
{
	currentFrame = 0u;
	nextFrameChange = SDL_GetTicks() + frameDuration;	
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
	writer.String("scale"); writer.Double(scale);
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
			for (unsigned j = 0u; j < 3u; ++j)
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
	scale = value["scale"].GetDouble();
	Reset(newNFrames, "");
	for (unsigned i = 0u; i < nFrames; ++i)
	{
		frames[i]->sprite = new Sprite();
		frames[i]->sprite->UnSerialize(value["frames"][i]["sprite"]);	
		for (unsigned j = 0u; j < 3u; ++j)
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

