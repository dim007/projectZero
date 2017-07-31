#include "stdafx.h"
#include "PauseState.h"
#include "utilities.h"
#include <iostream>
using namespace std;

PauseState::PauseState(StateStack& stack, Context context, StatesID::ID stateID) 
	: State(stack, context, stateID), mBackgroundSprite(), mPausedText(), mInstructionText()
{
	//lower volume
	mMusic = context.musicplayer;
	mMusic->setVolume(mMusic->getVolume() - 30);
	sf::Font& font = context.fonts->get(Fonts::Sansation);
	sf::Vector2f viewSize = context.window->getView().getSize();

	mPausedText.setFont(font);
	mPausedText.setString("Paused");
	mPausedText.setCharacterSize(70);
	centerOrigin(mPausedText);
	mPausedText.setPosition(0.5f * viewSize.x, 0.4f * viewSize.y);

	mInstructionText.setFont(font);
	mInstructionText.setString("Press BackSpace to return to main menu");
	centerOrigin(mInstructionText);
	mInstructionText.setPosition(0.5f * viewSize.x, 0.6f * viewSize.y);
}
void PauseState::draw() {
	sf::RenderWindow& window = *getContext().window;
	window.setView(window.getDefaultView());

	sf::RectangleShape backgroundShape;
	backgroundShape.setFillColor(sf::Color(0, 0, 0, 150));
	backgroundShape.setSize(window.getView().getSize());

	window.draw(backgroundShape);
	window.draw(mInstructionText);
	window.draw(mPausedText);
}
bool PauseState::update(sf::Time dt) {
	return false;
}
bool PauseState::handleEvent(const sf::Event& event) {
	if (event.type != sf::Event::KeyPressed)
		return false;
	//quit to main menu
	if (event.key.code == sf::Keyboard::BackSpace) {
		requestStateClear();
		requestStackPush(StatesID::Menu);
	}
	//unpause
	if (event.key.code == sf::Keyboard::Escape) {
		mMusic->setVolume(mMusic->getVolume() + 30);
		requestStackPop();
	}

	return true;

}