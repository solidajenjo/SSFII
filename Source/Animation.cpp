#include "Game.h"
#include "Globals.h"
#include "Render.h"
#include "Animation.h"
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
		nextFrameChange += frameDuration;
		++currentFrame;
		if (currentFrame >= nFrames)
		{
			currentFrame = 0u;
			loopEnded = true;
		}
	}
	game->render->RenderSprite(frames[currentFrame]->sprite, pos);
}
