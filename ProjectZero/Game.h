#ifndef GAME
#define GAME
#include "stdafx.h"
#include "ResourceHolder.h"
#include <SFML\Graphics.hpp>
#include "World.h"
#include "Player.h"


class Game
{
public:
	Game();
	void run();
	~Game();

private:
	void processInput();
	void update(sf::Time elaspedTime);
	void render();
	void updateStats(sf::Time elapsedTime);
	//void handlePlayerInput(sf::Keyboard::Key key, bool isPressed);
private:

	static const sf::Time TimePerFrame;
	sf::RenderWindow mWindow;
	//World mWorld;

	sf::Font mFont;
	sf::Text mStatsText;
	sf::Time mStatsUpdateTime;
	std::size_t mStatsNumFrames;
	Player mPlayer;
};

#endif