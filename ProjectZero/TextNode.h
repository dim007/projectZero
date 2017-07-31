#pragma once
#include "stdafx.h"
#include "SFML\Graphics.hpp"
#include "ResourceHolder.h"
#include "SceneNode.h"

class TextNode : public SceneNode {
public:
	typedef ResourceHolder<sf::Font, Fonts::ID> FontHolder;

	explicit TextNode(const FontHolder& fonts, const std::string& text);
	
	void setString(const std::string& text);

private:
	virtual void drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const;
private:
	sf::Text mText;
};