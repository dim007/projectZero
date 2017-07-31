#pragma once
#include "stdafx.h"
#include "StateStack.h"

class GameOverState : public State {
public:
	GameOverState(StateStack& stack, Context context, StatesID::ID stateID); //context is the struct holding all info, tetxures etc
	
	virtual void draw();
	virtual bool update(sf::Time dt);
	virtual bool handleEvent(const sf::Event& event);

private:
	sf::Text mGameOverText;
	sf::Time mElapsedTime;
	StatesID::ID mStateID;
};