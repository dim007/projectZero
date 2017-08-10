#include "stdafx.h"
#include "MenuState.h"
#include "utilities.h"
#include <memory>
#include <iostream>
#include "Player.h"
using namespace std;

MenuState::MenuState(StateStack& stack, Context context, StatesID::ID stateID)
	: State(stack, context, stateID), mGUIContainer(), blinkTime(sf::Time::Zero), typeTime(sf::Time::Zero)
{
	context.musicplayer->play(Music::MainMenu);
	context.musicplayer->setSkip(sf::seconds(25.2f));
	//PlayerLives renew
	Player& mPlayer = *getContext().player;
	mPlayer.setLives(2);

	typeCounter = 0;
	auto wSize = context.window->getView().getSize();

	myname.setFont(context.fonts->get(Fonts::Main));
	myname.setColor(sf::Color(0, 164, 0, 255));
	myname.setCharacterSize(75);
	myname.setString("Diego Martinez");
	centerOrigin(myname);
	myname.setPosition(wSize.x / 2.f, (wSize.y - (wSize.y / 4.f)));
	title1.setFont(context.fonts->get(Fonts::Main));
	title2.setFont(context.fonts->get(Fonts::Main));
	title1.setColor(sf::Color(0, 164, 0, 255));
	title2.setColor(sf::Color(0, 164, 0, 255));
	title1.setCharacterSize(100);
	title2.setCharacterSize(100);
	centerOrigin(title1); //move origin from top left corner to center of image
	centerOrigin(title2);
	title1.setString(">");
	title2.setString("_");
	
	title1.setPosition(wSize.x / 2.f, wSize.y / 4.f);
	title2.setPosition(wSize.x / 2.f, wSize.y / 4.f);

	auto playButton = std::make_shared<GUI::Button>(*context.fonts, *context.textures);
	sf::FloatRect bSize = playButton->mSprite.getLocalBounds();
	playButton->setPosition(wSize.x / 2.f - bSize.width / 2.f, 350);
	playButton->setText("Play");
	playButton->setCallback([this]() { //lambda expression
		requestStackPop();
		requestStackPush(StatesID::Game);
	});

	auto settingsButton = std::make_shared<GUI::Button>(*context.fonts, *context.textures);
	settingsButton->setPosition(wSize.x / 2.f - bSize.width / 2.f, 400);
	settingsButton->setText("Settings");
	settingsButton->setCallback([this]() {
		requestStackPush(StatesID::Settings);
	});

	auto exitButton = std::make_shared<GUI::Button>(*context.fonts, *context.textures);
	exitButton->setPosition(wSize.x / 2.f - bSize.width / 2.f, 450);
	exitButton->setText("Exit");
	exitButton->setCallback([this]() {
		requestStackPop();
	});

	mGUIContainer.pack(playButton);
	mGUIContainer.pack(settingsButton);
	mGUIContainer.pack(exitButton);
	playButton->select();
}
void MenuState::draw() {
	sf::RenderWindow& window = *getContext().window;
	window.setView(window.getDefaultView());

	//Draws blinking title black screen
	sf::RectangleShape blackBackground;
	blackBackground.setFillColor(sf::Color(0, 0, 0, 255));
	blackBackground.setSize(window.getView().getSize());
	window.draw(blackBackground);
	window.draw(title1);
	window.draw(myname);
	if (blink)
		window.draw(title2);

	window.draw(mGUIContainer);

}
bool MenuState::update(sf::Time dt) {
	blinkTime += dt;
	typeTime += dt;

	if (blinkTime >= sf::seconds(0.5f)) {
		blink = !blink;
		blinkTime = sf::Time::Zero;
	}
	if (typeTime >= sf::seconds(0.2f)) {
		typeTime = sf::Time::Zero;
		if (typeCounter < 11) {
			typeCounter++;
			updateTitle(typeCounter);
		}
	}
	return true;
}
bool MenuState::handleEvent(const sf::Event& event) {
	mGUIContainer.handleEvent(event);
	return false;
}
void MenuState::updateTitle(int count) {
	switch (count) {
	case 1:
		title1.setString(">p");
		break;
	case 2:
		title1.setString(">pr");
		break;
	case 3:
		title1.setString(">pro");
		break;
	case 4:
		title1.setString(">proj");
		break;
	case 5:
		title1.setString(">proje");
		break;
	case 6:
		title1.setString(">projec");
		break;
	case 7:
		title1.setString(">project");
		break;
	case 8:
		title1.setString(">projectZ");
		break;
	case 9:
		title1.setString(">projectZe");
		break;
	case 10:
		title1.setString(">projectZer");
		break;
	case 11:
		title1.setString(">projectZero");
		break;
	}
	centerOrigin(title1);
	//this adds the "_" to the very end of title text, fixes the shifting blink (cosmetic)
	auto pos = title1.getPosition();
	pos.x += title1.getLocalBounds().width / 2.f; //shift it by half the size of title1
	pos.y = title1.getPosition().y - 80;
	title2.setPosition(pos);
	//replaced alternating titles with two separate text boxes
	//centerOrigin(title2);
}

//Below is code with no buttons
/*MenuState::MenuState(StateStack& stack, Context context, StatesID::ID stateID)
: State(stack, context, stateID), mOptions(), mOptionIndex(0)
{
mBackgroundSprite.setTexture(context.textures->get(Textures::TitleScreen));
sf::Font& font = context.fonts->get(Fonts::Sansation);

//add the options to the vector mOptions
sf::Text playOption;
playOption.setFont(font);
playOption.setString("New Game");
centerOrigin(playOption);
playOption.setPosition(context.window->getView().getSize() / 2.f); //vector2f divided by 2
mOptions.push_back(playOption);

sf::Text exitOption;
exitOption.setFont(font);
exitOption.setString("Exit Game");
centerOrigin(exitOption);
exitOption.setPosition(playOption.getPosition() + sf::Vector2f(0.f, 30.f));
mOptions.push_back(exitOption);

updateOptionText();
}
void MenuState::draw() {
	sf::RenderWindow& window = *getContext().window;
	window.setView(window.getDefaultView());
	window.draw(mBackgroundSprite);
	for (const sf::Text& text : mOptions) {
		window.draw(text);
	}
}
bool MenuState::update(sf::Time dt) {
	return true;
}
bool MenuState::handleEvent(const sf::Event& event) {
	if (event.type != sf::Event::KeyPressed)
		return false;
	if (event.key.code == sf::Keyboard::Return) {
		if (mOptionIndex == Play) {
			requestStackPop();
			requestStackPush(StatesID::Game);
		}
		if (mOptionIndex == Exit) {
			requestStackPop();
		}
	}
	if (event.key.code == sf::Keyboard::Up) {
		//wrap around if at the top of option list
		if (mOptionIndex > 0)
			mOptionIndex--;
		else
			mOptionIndex = mOptions.size() - 1; //minus 1 for 0 index

		updateOptionText();
	}
	if (event.key.code == sf::Keyboard::Down) {
		if (mOptionIndex < mOptions.size() - 1)
			mOptionIndex++;
		else
			mOptionIndex = 0;
		updateOptionText();
	}
	return true;
}
void MenuState::updateOptionText() {
	if (mOptions.size() == 0)
		return;
	for (sf::Text& text : mOptions) {
		text.setColor(sf::Color::White);
	}
	mOptions[mOptionIndex].setColor(sf::Color::Red);

} */