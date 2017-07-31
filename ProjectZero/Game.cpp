#include "stdafx.h"
#include "Game.h"
#include <string>
#include <iostream>
#include <queue>

const sf::Time Game::TimePerFrame = sf::seconds(1.f / 60.f);

using namespace std;
/*
Game::Game()
	:mWindow(sf::VideoMode(640, 480), "Maverick", sf::Style::Close),
	mWorld(mWindow) new constructor, made this absolete
	mFont(),
	mStatsText(),
	mStatsUpdateTime(),
	mStatsNumFrames(0),
	mPlayer()
{
	mFont.loadFromFile("fonts/Sansation.ttf");
	mStatsText.setFont(mFont);
	mStatsText.setPosition(5.f, 5.f);
	mStatsText.setCharacterSize(10);
}

void Game::run() {
	sf::Clock clock;
	sf::Time timeSinceLastUpdate = sf::Time::Zero; //initialized to 0
	while (mWindow.isOpen()) {
		sf::Time elapsedTime = clock.restart(); //restarts clock, returns elapsedTime
		timeSinceLastUpdate += elapsedTime;
		while (timeSinceLastUpdate > TimePerFrame) { //limits framerate
			timeSinceLastUpdate -= TimePerFrame;
			processInput(); //take in input
			update(TimePerFrame); //updates position/moves character
		}
		//always run
		updateStats(elapsedTime);
		render(); //draws screen
	}
}

void Game::processInput() {
	std::queue<Command>& commands = mWorld.getCommandQueue();
	sf::Event event;
	while (mWindow.pollEvent(event)) {
		if (event.type == sf::Event::Closed)
			mWindow.close();
		mPlayer.handleEvent(event, commands);
	}
	mPlayer.handleRealTimeInput(commands);
}

void Game::update(sf::Time elapsedTime) {
	mWorld.update(elapsedTime);
	/*** OLD MOVEMENT CODE
	sf::Vector2f movement(0.f, 0.f);
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
		movement.y -= 30; //moves up
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
		movement.y += 30;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
		movement.x -= 30;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
		movement.x += 30;
	//distance = speed * time
	//Keeps movement uniform for every frame
	//mPlayer.move(movement * elapsedTime.asSeconds());
	
}
void Game::updateStats(sf::Time elapsedTime) {
	mStatsUpdateTime += elapsedTime;
	mStatsNumFrames += 1;

	if (mStatsUpdateTime >= sf::seconds(1.0f)) {
		mStatsText.setString(
			"Frames / Second = " + std::to_string(mStatsNumFrames) + "\n" +
			"Time / Update = " + std::to_string(mStatsUpdateTime.asMicroseconds() / mStatsNumFrames) + "us" + "\n");
		mStatsUpdateTime -= sf::seconds(1.0f);
		mStatsNumFrames = 0;
	}
}
void Game::render() {
	mWindow.clear();
	mWorld.draw(); //will draw SceneNodes

	mWindow.setView(mWindow.getDefaultView());
	mWindow.draw(mStatsText);
	mWindow.display();
}
Game::~Game()
{
}
*/