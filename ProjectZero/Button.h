#ifndef BUTTON
#define BUTTON
#include "stdafx.h"
#include "Component.h"
#include <functional>
#include "ResourceHolder.h"
namespace GUI {
	class Button : public Component {
	public:
		typedef std::shared_ptr<Button> Ptr;
		typedef std::function<void()> Callback;
		typedef ResourceHolder<sf::Texture, Textures::ID> TextureHolder; //shortcut for typing
		typedef ResourceHolder<sf::Font, Fonts::ID> FontHolder;
	public:
		Button(const FontHolder& fonts, const TextureHolder& textures);
		void setCallback(Callback callback);
		void setText(const std::string& text);
		void setToggle(bool flag);

		virtual bool isSelectable() const;
		virtual void select();
		virtual void deselect();
		
		virtual void activate(); //press
		virtual void deactivate(); //unpress

		virtual void handleEvent(const sf::Event& event);

	private:
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
	public:
		sf::Sprite mSprite;
	private:
		Callback mCallback;
		const sf::Texture& mNormalTexture;
		const sf::Texture& mSelectedTexture;
		const sf::Texture& mPressedTexture;
		sf::Text mText;
		bool mIsToggle;
	};
}
#endif