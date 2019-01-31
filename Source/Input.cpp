#include "Globals.h"
#include "Input.h"
#include "ExternalLibraries/SDL/include/SDL.h"
#include "ExternalLibraries/imgui/examples/imgui_impl_sdl.h"

#define MAX_KEYS 300
#define JOYSTICK_ACTIONS 12
#define JOYSTICK_DEAD_ZONE 8000

Input::Input() : mouse({0, 0}), mouse_motion({0,0})
{
	keyboard = new KeyState[MAX_KEYS];
	joystick = new KeyState[JOYSTICK_ACTIONS];
	memset(keyboard, KEY_IDLE, sizeof(KeyState) * MAX_KEYS);
	memset(mouse_buttons, KEY_IDLE, sizeof(KeyState) * NUM_MOUSE_BUTTONS);
	memset(joystick, KEY_IDLE, sizeof(KeyState) * JOYSTICK_ACTIONS);
}

// Destructor
Input::~Input()
{
	RELEASE_ARRAY(keyboard);
	RELEASE_ARRAY(joystick);
}

// Called before render is available
bool Input::Init()
{
	LOG("Init SDL input event system");
	bool ret = true;
	SDL_Init(0);

	if(SDL_InitSubSystem(SDL_INIT_EVENTS) < 0)
	{
		LOG("SDL_EVENTS could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	if (SDL_NumJoysticks() < 1)
	{
		LOG("Warning: No joysticks connected!");
	}
	else
	{
		//Load joystick
		gGameController = SDL_JoystickOpen(0);
		if (gGameController == NULL)
		{
			LOG("Warning: Unable to open game controller! SDL Error: %s\n", SDL_GetError());
		}
	}
	return ret;
}


// Called each loop iteration
bool Input::PreUpdate()
{
	static SDL_Event event;

	mouse_motion = {0, 0};
	memset(windowEvents, false, WE_COUNT * sizeof(bool));
	
	const Uint8* keys = SDL_GetKeyboardState(NULL);	
	for(int i = 0; i < MAX_KEYS; ++i)
	{
		if(keys[i] == 1)
		{
			if(keyboard[i] == KEY_IDLE)
				keyboard[i] = KEY_DOWN;
			else
				keyboard[i] = KEY_REPEAT;
		}
		else
		{
			if(keyboard[i] == KEY_REPEAT || keyboard[i] == KEY_DOWN)
				keyboard[i] = KEY_UP;
			else
				keyboard[i] = KEY_IDLE;
		}
	}
	

	for(int i = 0; i < NUM_MOUSE_BUTTONS; ++i)
	{
		if(mouse_buttons[i] == KEY_DOWN)
			mouse_buttons[i] = KEY_REPEAT;

		if(mouse_buttons[i] == KEY_UP)
			mouse_buttons[i] = KEY_IDLE;
	}

	while(SDL_PollEvent(&event) != 0)
	{
		
		ImGui_ImplSDL2_ProcessEvent(&event);
		switch(event.type)
		{
			case SDL_QUIT:
				windowEvents[WE_QUIT] = true;				
			break;

			case SDL_WINDOWEVENT:
				switch(event.window.event)
				{
					//case SDL_WINDOWEVENT_LEAVE:
					case SDL_WINDOWEVENT_HIDDEN:
					case SDL_WINDOWEVENT_MINIMIZED:
					case SDL_WINDOWEVENT_FOCUS_LOST:
					windowEvents[WE_HIDE] = true;
					break;

					//case SDL_WINDOWEVENT_ENTER:
					case SDL_WINDOWEVENT_SHOWN:
					case SDL_WINDOWEVENT_FOCUS_GAINED:
					case SDL_WINDOWEVENT_MAXIMIZED:
					case SDL_WINDOWEVENT_RESTORED:
					windowEvents[WE_SHOW] = true;
					break;
				}
			break;

			case SDL_MOUSEBUTTONDOWN:
				mouse_buttons[event.button.button - 1] = KEY_DOWN;
			break;

			case SDL_MOUSEBUTTONUP:
				mouse_buttons[event.button.button - 1] = KEY_UP;
			break;

			case SDL_MOUSEMOTION:
				mouse_motion.x = event.motion.xrel;
				mouse_motion.y = event.motion.yrel;
				mouse.x = event.motion.x;
				mouse.y = event.motion.y;
			break;

			case SDL_JOYBUTTONDOWN:
				LOG("%d", event.jbutton.button);
				if (joystick[event.jbutton.button] == KEY_DOWN)
					joystick[event.jbutton.button] = KEY_REPEAT;
				break;

			case SDL_JOYBUTTONUP:
				joystick[event.jbutton.button] = KEY_IDLE;
				break;

			case SDL_JOYAXISMOTION:
				if (event.jaxis.which == 0)
				{					
					if (event.jaxis.axis == 0)
					{
						//Left of dead zone
						if (event.jaxis.value < -JOYSTICK_DEAD_ZONE)
						{
							joystick[JOY_LEFT] = KEY_REPEAT;
						}
						//Right of dead zone
						else if (event.jaxis.value > JOYSTICK_DEAD_ZONE)
						{
							joystick[JOY_RIGHT] = KEY_REPEAT;
						}
						else
						{
							joystick[JOY_LEFT] = KEY_IDLE;
							joystick[JOY_RIGHT] = KEY_IDLE;
						}
					}
					if (event.jaxis.axis == 1)
					{
						//Left of dead zone
						if (event.jaxis.value < -JOYSTICK_DEAD_ZONE)
						{
							joystick[JOY_UP] = KEY_REPEAT;
						}
						//Right of dead zone
						else if (event.jaxis.value > JOYSTICK_DEAD_ZONE)
						{
							joystick[JOY_DOWN] = KEY_REPEAT;
						}
						else
						{
							joystick[JOY_UP] = KEY_IDLE;
							joystick[JOY_DOWN] = KEY_IDLE;
						}
					}
				}
				break;
		}
	}

	if(GetWindowEvent(EventWindow::WE_QUIT) == true || GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		return false;

	return true;
}

// Called before quitting
bool Input::Quit()
{
	LOG("Input quit");
	SDL_QuitSubSystem(SDL_INIT_EVENTS);
	return true;
}

// ---------
bool Input::GetWindowEvent(EventWindow ev) const
{
	return windowEvents[ev];
}

const iPoint& Input::GetMousePosition() const
{
	return mouse;
}

const iPoint& Input::GetMouseMotion() const
{
	return mouse_motion;
}