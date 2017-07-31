#include "stdafx.h"
#include "TitleState.h"
#include "utilities.h"

//State variable found in parent class
TitleState::TitleState(StateStack& stack, Context context, StatesID::ID stateID)
	: State(stack, context, stateID), mText(), mShowText(true), mTextEffectTime(sf::Time::Zero)
{
	context.musicplayer->play(Music::MainMenu);

	//mBackgroundSprite.setTexture();
	//using Text instead of image for title screen, saves memory
	mTitleText1.setFont(context.fonts->get(Fonts::Main));
	mTitleText2.setFont(context.fonts->get(Fonts::Main));
	mTitleText1.setString(">");
	mTitleText2.setString("_");
	mTitleText1.setCharacterSize(100);
	mTitleText2.setCharacterSize(100);
	mTitleText1.setColor(sf::Color(0, 164, 0, 255)); //terminal green
	mTitleText2.setColor(sf::Color(0, 164, 0, 255)); //terminal green
	centerOrigin(mTitleText1);
	centerOrigin(mTitleText2);
	mTitleText1.setPosition(context.window->getView().getSize().x / 2.f, context.window->getView().getSize().y / 4.f);
	mTitleText2.setPosition(context.window->getView().getSize().x / 2.f + 50.f, context.window->getView().getSize().y / 4.f + 20.f);


	mText.setFont(context.fonts->get(Fonts::Main));
	mText.setString("Press any key");
	centerOrigin(mText); //places the center of text at center instead of top left corner
	mText.setPosition(context.window->getView().getSize() / 2.f);
}
void TitleState::draw() {
	sf::RenderWindow& window = *getContext().window;
	if (mShowText) {
		window.draw(mTitleText2);
		window.draw(mText);
	}
	window.draw(mTitleText1);
}
bool TitleState::handleEvent(const sf::Event& event) {
	if (event.type == sf::Event::KeyPressed) {
		requestStackPop();
		requestStackPush(StatesID::Menu);
	}
	return true;
}
bool TitleState::update(sf::Time dt) {
	mTextEffectTime += dt;
	if (mTextEffectTime >= sf::seconds(0.5f)) {
		mShowText = !mShowText;
		mTextEffectTime = sf::Time::Zero;
	}
	return true;
}