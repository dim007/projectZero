#include "stdafx.h"
#include "Button.h"
#include "utilities.h"

namespace GUI {
	Button::Button(const FontHolder& fonts, const TextureHolder& textures)
		: mCallback(),
		mNormalTexture(textures.get(Textures::ButtonNormal)),
		mSelectedTexture(textures.get(Textures::ButtonSelected)),
		mPressedTexture(textures.get(Textures::ButtonPressed)),
		mSprite(),
		mText("", fonts.get(Fonts::Sansation), 16),
		mIsToggle(false)
	{
		mSprite.setTexture(mNormalTexture);
		sf::FloatRect bounds = mSprite.getLocalBounds();
		mText.setPosition(bounds.width / 2.f, bounds.height / 2.f);
	}
	void Button::setCallback(Callback callback) {
		mCallback = std::move(callback);
	}
	void Button::setText(const std::string& text) {
		mText.setString(text);
		centerOrigin(mText);
	}
	void Button::setToggle(bool flag) {
		mIsToggle = flag;
	}
	bool Button::isSelectable() const {
		return true;
	}
	void Button::select() {
		Component::select();
		mSprite.setTexture(mSelectedTexture);
	}
	void Button::deselect() {
		Component::deselect();
		mSprite.setTexture(mNormalTexture);
	}
	void Button::activate() {
		Component::activate();
		//if button is togglebutton we should show button pressed
		if (mIsToggle)
			mSprite.setTexture(mPressedTexture);
		if (mCallback && Component::isSelected())
			mCallback();
		//if button is not toggle, then deactivate btton
		if (!mIsToggle)
			deactivate();
	}
	void Button::deactivate() {
		Component::deactivate();
		if (mIsToggle) {
			//choose texture for toggle buttons
			if (isSelected())
				mSprite.setTexture(mSelectedTexture);
			else
				mSprite.setTexture(mNormalTexture);
		}
	}
	void Button::handleEvent(const sf::Event&) {
		//no variable in method def means empty?
	}
	void Button::draw(sf::RenderTarget& target, sf::RenderStates states) const {
		states.transform *= getTransform();
		target.draw(mSprite, states);
		target.draw(mText, states);
	}
}