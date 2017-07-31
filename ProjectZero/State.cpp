#include "stdafx.h"
#include "State.h"
#include "StateStack.h"
#include <iostream>
using namespace std;

//context called in Application.cpp
State::Context::Context(sf::RenderWindow& window, TextureHolder& textures, FontHolder& fonts, Player& player, MusicPlayer& musicplayer)
	: window(&window), textures(&textures), fonts(&fonts), player(&player), musicplayer(&musicplayer)
{
}
State::State(StateStack& stack, Context context, StatesID::ID stateID)
	: mStack(&stack), mContext(context), mStateID(stateID)
{
}
State::~State()
{
}
void State::requestStackPush(StatesID::ID stateID) {
	mStack->pushState(stateID);
}
void State::requestStackPop() {
	mStack->popState();
}
void State::requestStateClear() {
	mStack->clearStates();
}
State::Context State::getContext() const {
	return mContext;
}