#pragma once
#include "stdafx.h"
#include "Component.h"
#include "ResourceHolder.h"
#include "TextureID.h"

namespace GUI {
	class Label : public Component {
	public:
		typedef std::shared_ptr<Label> Ptr;
		typedef ResourceHolder<sf::Font, Fonts::ID> FontHolder;
	public:
		Label(const std::string& text, const FontHolder& fonts);
		virtual bool isSelectable() const;
		void setText(const std::string& text);
		
		virtual void handleEvent(const sf::Event& event);
	private:
		void draw(sf::RenderTarget& target, sf::RenderStates states) const;
		
	private:
		sf::Text mText;
	};
}