#pragma once
#include "stdafx.h"
#include "ResourceHolder.h"
#include "Entity.h"
#include <queue>


class Projectile : public Entity {
public:
	enum Type {
		AlliedBullet,
		EnemyBullet,
		Missile,
		TypeCount,
	};
public:
	typedef ResourceHolder<sf::Texture, Textures::ID> TextureHolder;

	Projectile(Type type, const TextureHolder& textures);
	
	void guideTowards(sf::Vector2f position);
	bool isGuided() const;
	virtual unsigned int getCategory() const;
	virtual sf::FloatRect getBoundingRect() const;
	float getMaxSpeed() const;
	int getDamage() const;

private:
	virtual void updateCurrent(sf::Time dt, std::queue<Command>& commands);
	virtual void drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const;

private:
	Type mType;
	sf::Sprite mSprite;
	Animation mAnim;
	sf::Vector2f mTargetDirection;
};