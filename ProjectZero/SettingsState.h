#pragma once
#include "stdafx.h"
#include "State.h"
#include "Button.h"
#include "Container.h"
#include "Label.h"
#include "Player.h"
#include <array>


class SettingsState : public State {
public:
	SettingsState(StateStack& stack, Context context, StatesID::ID stateID);
	virtual void draw();
	virtual bool update(sf::Time dt);
	virtual bool handleEvent(const sf::Event& event);
	std::string getKeyName(const sf::Keyboard::Key key);

private:
	void updateLabels();
	void addButtonLabel(Player::Action action, float y, const std::string& text, Context context);

private:
	sf::Sprite mBackgroundSprite;
	GUI::Container mGUIContainer;
	std::array<GUI::Button::Ptr, Player::ActionCount> mBindingButtons;
	std::array<GUI::Label::Ptr, Player::ActionCount> mBindingLabels;
};