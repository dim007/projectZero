#ifndef APPLICATION
#define APPLICATION
#include "stdafx.h"
#include "StateStack.h"
#include "SFML\System.hpp" //for time
#include "GameState.h"
#include "TitleState.h"
#include "MenuState.h"
#include "PauseState.h"
#include "SettingsState.h"
#include "GameOverState.h"
#include "MusicPlayer.h"

class Application {
	typedef ResourceHolder<sf::Texture, Textures::ID> TextureHolder; //shortcut for typing
	typedef ResourceHolder<sf::Font, Fonts::ID> FontHolder;
public:
	Application();
	void run();

private:
	void registerStates();
	void processInput();
	void update(sf::Time dt);
	void render();
	void updateStatistics(sf::Time dt);

private:
	static const sf::Time TimePerFrame;

	sf::RenderWindow mWindow;
	TextureHolder mTextures;
	FontHolder mFonts;
	Player mPlayer;
	MusicPlayer mMusicPlayer;
	
	StateStack mStateStack;
	sf::Text mStatisticsText;
	sf::Time mStatisticsUpdateTime;
	std::size_t mStatisticsNumFrames;
};

#endif