#include "stdafx.h"
#include "Application.h"
#include <string>

const sf::Time Application::TimePerFrame = sf::seconds(1.f / 60.f);

Application::Application() 
	: mWindow(sf::VideoMode(1024, 768), "States", sf::Style::Close),
	mTextures(),
	mFonts(),
	mPlayer(),
	mMusicPlayer(),
	mStateStack(State::Context(mWindow, mTextures, mFonts, mPlayer, mMusicPlayer)),
	mStatisticsText(),
	mStatisticsUpdateTime(),
	mStatisticsNumFrames(0)
{
	mWindow.setKeyRepeatEnabled(false); //if false, holding key is just 1 event not multiple
	//level textures are loaded in world.cpp
	mTextures.load(Textures::ButtonNormal, "textures/ButtonNormal.png");
	mTextures.load(Textures::ButtonPressed, "textures/ButtonPressed.png");
	mTextures.load(Textures::ButtonSelected, "textures/ButtonSelected.png");
	mFonts.load(Fonts::Sansation, "fonts/Sansation.ttf");
	mFonts.load(Fonts::Main, "fonts/clacon.ttf");
	mStatisticsText.setFont(mFonts.get(Fonts::Sansation));
	mStatisticsText.setPosition(5.f, 5.f);
	mStatisticsText.setCharacterSize(10u);

	registerStates();
	mStateStack.pushState(StatesID::Title);
}
void Application::run() {
	sf::Clock clock;
	sf::Time timeSinceLastUpdate = sf::Time::Zero; //initialized to 0
	while (mWindow.isOpen()) {
		sf::Time dt = clock.restart(); //restarts clock, returns elapsedTime
		timeSinceLastUpdate += dt;
		while (timeSinceLastUpdate >= TimePerFrame) { //limits framerate
			timeSinceLastUpdate -= TimePerFrame;

			processInput(); //take in input
			update(TimePerFrame); //updates position/moves character
			if (mStateStack.isEmpty())
				mWindow.close();
		}
		updateStatistics(dt);
		render(); //draws screen
	}
}
void Application::processInput() {
	sf::Event event;
	while (mWindow.pollEvent(event)) {
		mStateStack.handleEvent(event);
		if (event.type == sf::Event::Closed)
			mWindow.close();
	}
}
void Application::update(sf::Time dt) {
	mStateStack.update(dt);
}
void Application::render() {
	mWindow.clear();
	mStateStack.draw();

	mWindow.setView(mWindow.getDefaultView());
	mWindow.draw(mStatisticsText);

	mWindow.display();
}
void Application::updateStatistics(sf::Time dt) {
	mStatisticsUpdateTime += dt;
	mStatisticsNumFrames += 1;
	if (mStatisticsUpdateTime >= sf::seconds(1.0f)) {
		mStatisticsText.setString("FPS: " + std::to_string(mStatisticsNumFrames));

		mStatisticsUpdateTime -= sf::seconds(1.0f);
		mStatisticsNumFrames = 0;
	}
}
void Application::registerStates() {
	mStateStack.registerState<TitleState>(StatesID::Title);
	mStateStack.registerState<MenuState>(StatesID::Menu);
	mStateStack.registerState<GameState>(StatesID::Game);
	mStateStack.registerState<PauseState>(StatesID::Pause);
	mStateStack.registerState<SettingsState>(StatesID::Settings);
	mStateStack.registerState<GameOverState>(StatesID::GameOver);
}