#pragma once
#include "stdafx.h"
#include "ResourceHolder.h"
#include "TextureID.h"
#include "Entity.h"
#include "SFML\Graphics.hpp"


class Robot;
class Pickup : public Entity {
public:
	enum Type {
		HealthRefill,
		MissileRefill,
		TypeCount
	};
public:
	typedef ResourceHolder<sf::Texture, Textures::ID> TextureHolder;
	Pickup(Type type, const TextureHolder& textures);

	virtual unsigned int getCategory() const;
	virtual sf::FloatRect getBoundingRect() const;
	void apply(Robot& player) const;

protected:
	virtual void drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const;

private:
	Type mType;
	sf::Sprite mSprite;
};