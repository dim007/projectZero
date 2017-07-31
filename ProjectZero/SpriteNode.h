#ifndef SPRITENODE
#define SPRITENODE
#include "stdafx.h"
#include "SceneNode.h"
#include "SFML\Graphics.hpp"
#include "Category.h"

//SpriteNode is used for textures in backgrounds
class SpriteNode : public SceneNode {
public:
	explicit SpriteNode(const sf::Texture& texture);
	SpriteNode(const sf::Texture& texture, const sf::IntRect& rect);
private:
	virtual void drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const;
	sf::Sprite mSprite;
};
#endif