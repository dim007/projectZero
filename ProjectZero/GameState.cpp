#include "stdafx.h"
#include "GameState.h"
#include <iostream>
using namespace std;

GameState::GameState(StateStack& stack, Context context, StatesID::ID stateID)
	: State(stack, context, stateID),
	mWorld(*context.window, *context.fonts),
	mPlayer(*context.player)
{
	context.musicplayer->play(Music::Level1);
	context.musicplayer->setVolume(50);
}
void GameState::draw() {
	mWorld.draw();
}
bool GameState::update(sf::Time dt) {
	mWorld.update(dt);
	mWorld.setPlayerLives(mPlayer.getLives());

	if (!mWorld.hasAlivePlayer()) {
		mPlayer.setMissionStatus(Player::MissionFailure);
		mPlayer.setLives(mPlayer.getLives() - 1);
		requestStackPush(StatesID::GameOver);
	}
	else if (mWorld.hasPlayerReachedEnd()) {
		mPlayer.setMissionStatus(Player::MissionSuccess);
		requestStackPush(StatesID::GameOver);
	}
	std::queue<Command>& CommandQ = mWorld.getCommandQueue();
	mPlayer.handleRealTimeInput(CommandQ);

	return true;
}
bool GameState::handleEvent(const sf::Event& event) {
	//game input
	std::queue<Command>& CommandQ = mWorld.getCommandQueue();
	mPlayer.handleEvent(event, CommandQ);

	//Pause screen trigger
	if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
		requestStackPush(StatesID::Pause);
	}

	return true;
}