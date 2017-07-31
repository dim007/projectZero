#include "stdafx.h"
#include "StateStack.h"
#include <cassert>
#include <iostream> 
using namespace std;
//passing in struct Context, from state.h
StateStack::PendingChange::PendingChange(Action action, StatesID::ID stateID)
	: action(action), stateID(stateID) 
{
}
StateStack::PendingChange::PendingChange(Action action)
	: action(action)
{
}
StateStack::StateStack(State::Context context) :
	mStack(), 
	mPendingList(), 
	mContext(context), 
	mFactories()
{
	isPause = false;
}
State::Ptr StateStack::createState(StatesID::ID stateID) {
	auto found = mFactories.find(stateID);
	assert(found != mFactories.end());
	return found->second(); //returns ptr to base state class
}
void StateStack::handleEvent(const sf::Event& event) {
	//iterate from highest to lowest state
	for (auto itr = mStack.rbegin(); itr != mStack.rend(); itr++) {
		if (!(*itr)->handleEvent(event)) //returns when an event returns false
			return;
		//gives control to the states that dont want to let input flow to other states besides itself
	}
	applyPendingChanges();
}
void StateStack::update(sf::Time dt) {
	//iterates from top to bottom, stop if returns false
	//rbegin reverse iterator
	for (auto itr = mStack.rbegin(); itr != mStack.rend(); ++itr) {
		if (!(*itr)->update(dt))
			break;
	}
	applyPendingChanges();
}
void StateStack::draw() {
//draw states from lowest & oldest, then others, so current state overlays ontop of others
	//from bottom to top
	for (State::Ptr& state : mStack)
	{
		state->draw();
	}
}
void StateStack::pushState(StatesID::ID stateID) {
	//pending change struct, (Action, stateID)
	if (stateID == StatesID::Pause && isPause)
		return;
	else if (stateID == StatesID::Pause && !isPause)
		isPause = true;
	mPendingList.push_back(PendingChange(Push, stateID));
}
void StateStack::popState() {
	mPendingList.push_back(PendingChange(Pop));
}
void StateStack::clearStates() {
	mPendingList.push_back(PendingChange(Clear));
}
bool StateStack::isEmpty() const {
	return mStack.empty();
}
void StateStack::applyPendingChanges() {
	//allows for uniterruption of handleEvent and update
	for each (PendingChange change in mPendingList)
	{
		switch (change.action) {
		case Push:
			mStack.push_back(createState(change.stateID));
			break;
		case Pop:
			if (mStack.back()->mStateID == StatesID::Pause)
				isPause = false;
			mStack.pop_back();
			break;
		case Clear:
			mStack.clear();
			isPause = false;
			break;
		}
	}
	mPendingList.clear();
}