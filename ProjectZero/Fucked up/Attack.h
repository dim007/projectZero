#pragma once
#include "stdafx.h"
#include "Entity.h"
#include "ResourceHolder.h"

class Attack : public Entity {
public:
	enum Type {
		Slash1,
		Slash2,
		Slash3,
	};
	typedef ResourceHolder<sf::Texture, Textures::ID> TextureHolder;

	Attack(Type type, const TextureHolder& textures);

	virtual unsigned int getCategory() const;
	Textures::ID getTextureID();
	virtual sf::FloatRect getBoundingRect() const;
	int getDamage() const;
	sf::Time getDuration();

private:
	virtual void updateCurrent(sf::Time dt, std::queue<Command>& commands);
	virtual void drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const;

private:
	Type mType;
	sf::Sprite mSprite;
	sf::Time attackDuration;
};