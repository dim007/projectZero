#include "stdafx.h"
#include "Attack.h"
#include "utilities.h"
#include <iostream>

Attack::Attack(Type type, const TextureHolder& textures, float& sign)
	: Entity(1),
	mType(type),
	attackDuration(sf::Time::Zero)
{
	attackData = setInfo();
	mHitBox.setTexture(textures.get(attackData[mType].textureID));
	mHitBox.setTextureRect(attackData[mType].hitBox);
	centerOrigin(mHitBox);

	mAnim.setTexture(textures.get(attackData[mType].textureID));
	mAnim.setVectorFrames(attackData[mType].vectorFrames);
	mAnim.setNumFrames(attackData[mType].numFrames);
	mAnim.setDuration(attackData[mType].duration);
	mAnim.setSign(sign);
	mAnim.update(sf::seconds(0.001f));
}

unsigned int Attack::getCategory() const {
	return attackData[mType].category;
}
sf::FloatRect Attack::getBoundingRect() const {
	sf::Transform trans = getWorldTransform(); //transform from here to root
	return trans.transformRect(mHitBox.getGlobalBounds()); //Transform the sprites bounds using the transform
}
int Attack::getDamage() const {
	return attackData[mType].damage;
}
void Attack::updateCurrent(sf::Time dt, std::queue<Command>& commands) {
	if (attackDuration >= attackData[mType].duration)
		destroy();
	mAnim.update(dt);
	Entity::updateCurrent(dt, commands);
	attackDuration += dt;
}
void Attack::drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const {
	if (!attackData[mType].noAttackAnim)
		target.draw(mAnim, states);
}

vector<attackInfo> Attack::setInfo() {
	vector<attackInfo> data(Attack::TypeCount);
	data[Attack::Slash1].category = Category::AlliedAttack;
	data[Attack::Slash1].damage = 6;
	data[Attack::Slash1].duration = sf::seconds(0.25f);
	data[Attack::Slash1].textureID = Textures::Attack;
	data[Attack::Slash1].hitBox = sf::IntRect(0,0,40,61);
	data[Attack::Slash1].numFrames = 4;
	vector<sf::IntRect> frames(5);
	frames[0] = sf::IntRect(0, 0, 44, 62);
	frames[1] = sf::IntRect(44, 0, 39, 62);
	frames[2] = sf::IntRect(83, 0, 39, 62);
	frames[3] = sf::IntRect(122, 0, 38, 62);
	data[Attack::Slash1].vectorFrames = frames;
	data[Attack::slashSuit].noAttackAnim = false;

	data[Attack::Slash2].category = Category::AlliedAttack;
	data[Attack::Slash2].damage = 6;
	data[Attack::Slash2].duration = sf::seconds(0.2f);
	data[Attack::Slash2].textureID = Textures::Attack;
	data[Attack::Slash2].hitBox = sf::IntRect(0, 0, 68, 16);
	data[Attack::Slash2].numFrames = 2;
	frames[0] = sf::IntRect(177, 44, 68, 12);
	frames[1] = sf::IntRect(245, 43, 68, 16);
	data[Attack::Slash2].vectorFrames = frames;
	data[Attack::slashSuit].noAttackAnim = false;

	data[Attack::Slash3].category = Category::AlliedAttack;
	data[Attack::Slash3].damage = 6;
	data[Attack::Slash3].duration = sf::seconds(0.3f);
	data[Attack::Slash3].textureID = Textures::Attack;
	data[Attack::Slash3].hitBox = sf::IntRect(0, 0, 70, 73);
	data[Attack::Slash3].numFrames = 4;
	frames[0] = sf::IntRect(3, 77, 84, 73);
	frames[1] = sf::IntRect(94, 77, 69, 73);
	frames[2] = sf::IntRect(168, 77, 70, 73);
	frames[3] = sf::IntRect(243, 77, 70, 73);
	data[Attack::Slash3].vectorFrames = frames;
	data[Attack::slashSuit].noAttackAnim = false;

	data[Attack::jumpSlash].category = Category::AlliedAttack;
	data[Attack::jumpSlash].damage = 6;
	data[Attack::jumpSlash].duration = sf::seconds(0.15f);
	data[Attack::jumpSlash].textureID = Textures::Attack;
	data[Attack::jumpSlash].hitBox = sf::IntRect(0, 0, 100, 60);
	data[Attack::jumpSlash].numFrames = 3;
	frames[0] = sf::IntRect(2, 151, 43, 60);
	frames[1] = sf::IntRect(46, 151, 73, 60);
	frames[2] = sf::IntRect(128, 151, 100, 60);
	data[Attack::jumpSlash].vectorFrames = frames;
	data[Attack::slashSuit].noAttackAnim = false;

	data[Attack::hangSlash].category = Category::AlliedAttack;
	data[Attack::hangSlash].damage = 6;
	data[Attack::hangSlash].duration = sf::seconds(0.3f);
	data[Attack::hangSlash].textureID = Textures::Attack;
	data[Attack::hangSlash].hitBox = sf::IntRect(0, 0, 62, 44);
	data[Attack::hangSlash].numFrames = 4;
	frames[0] = sf::IntRect(2, 209, 58, 44);
	frames[1] = sf::IntRect(70, 209, 62, 44);
	frames[2] = sf::IntRect(140, 209, 61, 44);
	frames[3] = sf::IntRect(213, 209, 41, 44);
	data[Attack::hangSlash].vectorFrames = frames;
	data[Attack::slashSuit].noAttackAnim = false;

	data[Attack::slashSuit].category = Category::EnemyAttack;
	data[Attack::slashSuit].damage = 3;
	data[Attack::slashSuit].duration = sf::seconds(0.3f);
	data[Attack::slashSuit].textureID = Textures::Enemies;
	data[Attack::slashSuit].hitBox = sf::IntRect(0, 0, 69, 107);
	//data[Attack::slashSuit].numFrames = 1;
	//frames[0] = sf::IntRect(312, 126, 69, 107);
	data[Attack::slashSuit].vectorFrames = frames;
	data[Attack::slashSuit].noAttackAnim = true;

	return data;
}