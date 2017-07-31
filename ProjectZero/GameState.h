#ifndef GAMESTATE
#define GAMESTATE
#include "stdafx.h"
#include "World.h"
#include "Player.h"
#include "State.h"
#include "SFML\System.hpp" //for time

class GameState : public State {
public:
	GameState(StateStack& stack, Context context, StatesID::ID stateID);
	void draw();
	bool update(sf::Time dt);
	bool handleEvent(const sf::Event& event);
private:
	World mWorld;
	Player& mPlayer;
};

#endif