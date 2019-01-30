#ifndef __ANIMATION_H_
#define __ANIMATION_H_
#include "ExternalLibraries/MathGeoLib/include/Math/float3.h"
#include "ExternalLibraries/rapidjson-1.1.0/include/rapidjson/prettywriter.h"
#include "ExternalLibraries/rapidjson-1.1.0/include/rapidjson/document.h"
#include "ExternalLibraries/MathGeoLib/include/Geometry/AABB2D.h"

class Sprite;

class Animation
{
public:
	struct HitBox
	{
		AABB2D box;		
		int damageAmount = 0;
	};

	struct Frame
	{
		Sprite* sprite;
		HitBox hitBoxes[4]; // 0 - Face / 1 - Body / 2 - Legs / 3 - Attack
		int offsetH = 0;
		int offsetV = 0;
	};

	Animation(unsigned nFrames);
	~Animation();

	bool SetFrame(unsigned frameNum, Sprite* sprite);
	void Play(const float3 &pos, bool &loopEnded, bool loop = true);
	void Rewind();

	void Reset(unsigned newNFrames, std::string sheetPath);
	void Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer) const;
	void UnSerialize(rapidjson::Value &value);


	//members

	Frame** frames = nullptr;
	unsigned nFrames = 0u, currentFrame = 0u;
	unsigned frameDuration = 400u;
	unsigned lastTime = 0u;
	unsigned nextFrameChange = frameDuration;
	bool reverse = false;
	std::string name = "";
};


#endif