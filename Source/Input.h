#ifndef __INPUT_H__
#define __INPUT_H__

#include "Point.h"

#include "ExternalLibraries/SDL/include/SDL_scancode.h"
#include "ExternalLibraries/SDL/include/SDL_joystick.h"

#define NUM_MOUSE_BUTTONS 5

enum EventWindow
{
	WE_QUIT = 0,
	WE_HIDE = 1,
	WE_SHOW = 2,
	WE_COUNT
};

enum KeyState
{
	KEY_IDLE = 0,
	KEY_DOWN,
	KEY_REPEAT,
	KEY_UP
};

enum Joystick
{
	JOY_BUTTON_A = 0,
	JOY_BUTTON_B,
	JOY_BUTTON_X,
	JOY_BUTTON_Y,
	JOY_BUTTON_LB,
	JOY_BUTTON_RB,
	JOY_BUTTON_SELECT,
	JOY_BUTTON_START,
	JOY_LEFT,
	JOY_RIGHT,
	JOY_UP,
	JOY_DOWN,
};

class Input
{

public:

	Input();

	// Destructor
	virtual ~Input();

	// Called before render is available
	bool Init();


	// Called each loop iteration
	bool PreUpdate();

	// Called before quitting
	bool Quit();

	// Check key states (includes mouse and joy buttons)
	KeyState GetKey(int id) const
	{
		return keyboard[id];
	}

	KeyState GetJoy(int id) const
	{
		return joystick[id];
	}

	KeyState GetMouseButtonDown(int id) const
	{
		return mouse_buttons[id - 1];
	}

	// Check for window events last frame
	bool GetWindowEvent(EventWindow code) const;

	// Get mouse / axis position
	const iPoint& GetMouseMotion() const;
	const iPoint& GetMousePosition() const;

private:
	bool		windowEvents[WE_COUNT];
	KeyState*	keyboard;
	KeyState*	joystick;
	KeyState	mouse_buttons[NUM_MOUSE_BUTTONS];
	iPoint mouse_motion;
	iPoint mouse;
	SDL_Joystick* gGameController = nullptr;
};

#endif // __MODULEINPUT_H__