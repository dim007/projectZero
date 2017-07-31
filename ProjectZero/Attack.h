#pragma once
#include "stdafx.h"
#include "Entity.h"
#include "ResourceHolder.h"
#include <vector>
using namespace std;

struct attackInfo {
	Textures::ID textureID;
	sf::Time duration;
	int damage;
	unsigned int category;
	sf::IntRect hitBox;
	vector<sf::IntRect> vectorFrames;
	int numFrames;
	bool noAttackAnim;
};
class Attack : public Entity {
public:
	enum Type {
		Slash1,
		Slash2,
		Slash3,
		jumpSlash,
		hangSlash,
		slashSuit,
		TypeCount
	};
	typedef ResourceHolder<sf::Texture, Textures::ID> TextureHolder;

	Attack(Type type, const TextureHolder& textures, float& sign);

	virtual unsigned int getCategory() const;
	virtual sf::FloatRect getBoundingRect() const;
	int getDamage() const;

private:
	virtual void updateCurrent(sf::Time dt, std::queue<Command>& commands);
	virtual void drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const;
	vector<attackInfo> setInfo();
private:
	Type mType;
	sf::Sprite mHitBox;
	Animation mAnim;
	sf::Time attackDuration;
	vector<attackInfo> attackData;
	float* mSign;
};