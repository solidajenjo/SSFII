#ifndef _GLOBALS_H
#define _GLOBALS_H

#include <windows.h>
#include <stdio.h>

#define LOG(format, ...) log(__FILE__, __LINE__, format, __VA_ARGS__)
void log(const char file[], int line, const char* format, ...);

#define RELEASE(p) { \
	if (p != nullptr) \
	{ \
		delete p; \
		p = nullptr; \
	} \
}

// Deletes an array of buffers
#define RELEASE_ARRAY( x ) \
	{\
       if( x != nullptr )\
       {\
           delete[] x;\
	       x = nullptr;\
		 }\
	 }


enum LoopStates
{
	PREUPDATE,
	UPDATE,
	POSTUPDATE,
	QUIT,
	END
};
// Configuration -----------

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080
#define ANIM_NUM 43
#endif