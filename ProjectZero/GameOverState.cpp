#include "stdafx.h"
#include "GameOverState.h"
#include "utilities.h"
#include "Player.h"

GameOverState::GameOverState(StateStack& stack, Context context, StatesID::ID stateID)
	: State(stack, context, StatesID::GameOver),
	mGameOverText(),
	mElapsedTime(sf::Time::Zero),
	mStateID(stateID)
{
	context.musicplayer->stop();

	sf::Font& font = context.fonts->get(Fonts::Main);
	sf::Vector2f windowSize(context.window->getSize());

	mGameOverText.setFont(font);
	if (context.player->getMissionStatus() == Player::MissionFailure)
		mGameOverText.setString("Wasted");
	else
		mGameOverText.setString("Mission Complete");

	mGameOverText.setCharacterSize(70);
	centerOrigin(mGameOverText);
	mGameOverText.setPosition(0.5f * windowSize.x, 0.4f * windowSize.y);
}
void GameOverState::draw() {
	sf::RenderWindow& window = *getContext().window;
	window.setView(window.getDefaultView());

	//dark transparent background
	sf::RectangleShape backgroundShape;
	backgroundShape.setFillColor(sf::Color(0, 0, 0, 150)); //RGB,Alpha
	backgroundShape.setSize(window.getView().getSize());

	window.draw(backgroundShape);
	window.draw(mGameOverText);
}
bool GameOverState::update(sf::Time dt) {
	mElapsedTime += dt;
	Player mPlayer = *getContext().player;
	if (mElapsedTime >= sf::seconds(2.f)) {
		requestStateClear();
		if (mPlayer.getLives() >= 0)
			requestStackPush(StatesID::Game);
		else
			requestStackPush(StatesID::Menu);
	}
	return false;
}
bool GameOverState::handleEvent(const sf::Event&) {
	return false;
}