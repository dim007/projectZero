#ifndef PAUSESTATE
#define PAUSESTATE
#include "stdafx.h"
#include "State.h"
#include "GameState.h"

class PauseState : public State {
public:
	PauseState(StateStack& stack, Context context, StatesID::ID stateID);
	void draw();
	bool update(sf::Time dt);
	bool handleEvent(const sf::Event& event);

private:
	MusicPlayer* mMusic;
	sf::Sprite mBackgroundSprite;
	sf::Text mPausedText;
	sf::Text mInstructionText;
};

#endif