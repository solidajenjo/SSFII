#include "Game.h"
#include "Globals.h"

Game* game = nullptr;

int main(int argc, char* argv[])
{
	game = new Game();
	LoopStates state = LoopStates::END;

	if (game->Init())
		state = LoopStates::PREUPDATE;

	while (state != LoopStates::END)
	{
		switch (state)
		{
		case LoopStates::PREUPDATE:
			if (game->PreUpdate())
				state = LoopStates::UPDATE;
			else
				state = LoopStates::QUIT;
			break;
		case LoopStates::UPDATE:
			if (game->Update())
				state = LoopStates::POSTUPDATE;
			else
				state = LoopStates::QUIT;
			break;
		case LoopStates::POSTUPDATE:
			if (game->PostUpdate())
				state = LoopStates::PREUPDATE;
			else
				state = LoopStates::QUIT;
			break;
		case LoopStates::QUIT:
			game->Quit();
			state = LoopStates::END;
		}
	}
}