#include "stdafx.h"
#include "Attack.h"
#include "utilities.h"

Attack::Attack(Type type, const TextureHolder& textures)
	: Entity(1),
	mType(type),
	attackDuration(sf::Time::Zero)
{
	//Textures::ID id = getTextureID();
	//mSprite.setTexture(textures.get(id));
	centerOrigin(mSprite);
}

unsigned int Attack::getCategory() const {
	switch (mType) {
	case Slash1 || Slash2 || Slash3:
		return Category::AlliedAttack;
	default:
		return Category::EnemyAttack;
	}
}

sf::FloatRect Attack::getBoundingRect() const {
	sf::Transform trans = getWorldTransform(); //transform from here to root
	return trans.transformRect(mSprite.getGlobalBounds()); //Transform the sprites bounds using the transform
}
int Attack::getDamage() const {
	switch (mType) {
	case Slash1 || Slash2 || Slash3:
		return 100;
	default:
		return 50;
	}
}
void Attack::updateCurrent(sf::Time dt, std::queue<Command>& commands) {
	return;
	if (attackDuration >= getDuration())
		destroy();
	Entity::updateCurrent(dt, commands);
	attackDuration += dt;
}
void Attack::drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const {
	return;
	target.draw(mSprite, states);
}
sf::Time Attack::getDuration() {
	switch (mType) {
	case Slash1 || Slash2 || Slash3:
		return sf::seconds(0.5);
	default:
		return sf::seconds(0.5);
	}
}
Textures::ID Attack::getTextureID() {
	switch (mType) {
	case Slash1:
		return Textures::Slash1;
	case Slash2:
		return Textures::Slash2;
	case Slash3:
		return Textures::Slash3;
	default:
		return Textures::Zero;
	}
}