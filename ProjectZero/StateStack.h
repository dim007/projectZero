#ifndef STATESTACK
#define STATESTACK
#include "stdafx.h"
#include "SFML\System.hpp"
#include <vector>
#include <map>
#include "State.h"
#include "SFML\Graphics.hpp" //for sf::Event
#include <functional>

namespace sf {
	class Event;
	class RenderWindow;
}

class StateStack : private sf::NonCopyable {
public:
	enum Action {
		Push,
		Pop,
		Clear,
	};
public: 
	explicit StateStack(State::Context context);

	template<typename T>
	void registerState(StatesID::ID stateID);
	void update(sf::Time dt);
	void draw();
	void handleEvent(const sf::Event& event);

	void pushState(StatesID::ID stateID);
	void popState();
	void clearStates();

	bool isEmpty() const;
	bool isPause;

private:
	State::Ptr createState(StatesID::ID stateID);
	void applyPendingChanges();
private:
	struct PendingChange {
		explicit PendingChange(Action action, StatesID::ID stateID);
		explicit PendingChange(Action action);
		Action action;
		StatesID::ID stateID;
	};
	
	//Vars
	std::vector<State::Ptr> mStack;
	std::vector<PendingChange> mPendingList;
	State::Context mContext;
	std::map<StatesID::ID, std::function<State::Ptr()>> mFactories; //creates states on demand
};

template <typename T>
void StateStack::registerState(StatesID::ID stateID) {
	//lamda function creates state
	mFactories[stateID] = [this, stateID]() { //added stateID into brackets to implicitly capture for lambda
		return State::Ptr(new T(*this, mContext, stateID));
	};
}
#endif