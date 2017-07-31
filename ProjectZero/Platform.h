#pragma once
#include "stdafx.h"
#include "SceneNode.h"
#include "ResourceHolder.h"
#include "TextureID.h"

class Platform : public SceneNode {
public:
	enum Type {
		Highway
	};
public:
	typedef ResourceHolder<sf::Texture, Textures::ID> TextureHolder;
	explicit Platform(Platform::Type type, TextureHolder& textures, sf::IntRect size);
	Textures::ID getTextureID();
	virtual unsigned int getCategory() const; //Needed const for Collision
	virtual sf::FloatRect getBoundingRect() const; // const For collision

private:
	virtual void drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const; //needs on else SceneNode calls draw, redraws 2x
	//virtual sf::FloatRect getBoundingRect() const;

private:
	Type mType;
	sf::Sprite mSprite;
};
