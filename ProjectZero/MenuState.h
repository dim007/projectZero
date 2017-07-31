#pragma once
#ifndef MENUSTATE
#define MENUSTATE
#include "stdafx.h"
#include "State.h"
#include <vector>
#include "Container.h"
#include "Button.h"

class MenuState : public State {
public:
	MenuState(StateStack& stack, Context context, StatesID::ID stateID);
	void draw();
	bool update(sf::Time dt);
	bool handleEvent(const sf::Event& event);

	//void updateOptionText();

private:
	enum OptionNames {
		Play, Exit
	};
private:
	//std::vector<sf::Text> mOptions; //old menu
	//std::size_t mOptionIndex;
	GUI::Container mGUIContainer;
	void updateTitle(int count);

	bool blink;
	sf::Time blinkTime;
	sf::Time typeTime;
	int typeCounter;
	sf::Text title1;
	sf::Text title2;

	//sf::Sprite titleSprite1;
	//sf::Sprite titleSprite2;
};

#endif