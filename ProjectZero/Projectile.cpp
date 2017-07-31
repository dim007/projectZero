#include "stdafx.h"
#include "Projectile.h"
#include "utilities.h"
#include "DataTables.h"
#include "DataTables.h"

namespace
{
	//needed for glabal table
	const std::vector<ProjectileData> Table = initializeProjectileData();
}
Projectile::Projectile(Type type, const TextureHolder& textures)
	: Entity(1), //hp
	mType(type),
	mSprite(textures.get(Table[type].texture))
{
	centerOrigin(mSprite);
}

void Projectile::guideTowards(sf::Vector2f position) {
	assert(isGuided());
	//unitvector found in utilities.h
	//unitVector = vector / length(vector)
	mTargetDirection = unitVector(position - getWorldPosition());
	mTargetDirection.x = mTargetDirection.x * 3;
	mTargetDirection.y = mTargetDirection.y * 3;
}
bool Projectile::isGuided() const {
	return mType == Missile;
}
void Projectile::updateCurrent(sf::Time dt, std::queue<Command>& commands) {
	if (isGuided()) {
		const float approachRate = 300.f;

		sf::Vector2f newVelocity = unitVector(approachRate * dt.asSeconds() * mTargetDirection + getVelocity());
		newVelocity *= getMaxSpeed();
		float angle = std::atan2(newVelocity.y, newVelocity.x);

		setRotation(toDegree(angle));
		setVelocity(newVelocity);
	}
	Entity::updateCurrent(dt, commands);
}
void Projectile::drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const {
	target.draw(mSprite, states);
}
unsigned int Projectile::getCategory() const {
	if (mType == EnemyBullet)
		return Category::EnemyProjectile; //found in command.h
	else
		return Category::AlliedProjectile;
}
sf::FloatRect Projectile::getBoundingRect() const {
	return getWorldTransform().transformRect(mSprite.getGlobalBounds());
}

float Projectile::getMaxSpeed() const {
	return Table[mType].speed;
}
int Projectile::getDamage() const {
	return Table[mType].damage;
}