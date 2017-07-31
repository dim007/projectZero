#include "stdafx.h"
#include "Entity.h"
#include <iostream>
using namespace std;

Entity::Entity(int hitpoints)
	: mHitpoints(hitpoints)
{
}
void Entity::setLives(int lives) {
	mLives = lives;
}
int Entity::getLives() {
	return mLives;
}
void Entity::repair(int points) {
	assert(points > 0);
	mHitpoints += points;
}
void Entity::damage(int points) {
	assert(points > 0);
	mHitpoints -= points;
}
void Entity::destroy() {
	mHitpoints = 0;
	mLives--;
}
void Entity::setHitpoints(int hits) {
	mHitpoints = hits;
}
int Entity::getHitpoints() const {
	return mHitpoints;
}
bool Entity::isDestroyed() const {
	if (mHitpoints <= 0)
		return true;
	return false;
}
void Entity::setVelocity(sf::Vector2f velocity) {
	mVelocity = velocity;
}
void Entity::setVelocity(float vx, float vy) {
	mVelocity.x = vx;
	mVelocity.y = vy;
}
sf::Vector2f Entity::getVelocity() const{
	return mVelocity;
}
void Entity::accelerate(sf::Vector2f velocity) {
	mVelocity += velocity;
}
void Entity::accelerate(float vx, float vy) {
	mVelocity.x += vx;
	mVelocity.y += vy;
}
void Entity::updateCurrent(sf::Time dt, std::queue<Command> commands) {
	//sf::Vector2f oldPosition = getPosition();

	//update movements
	move(mVelocity * dt.asSeconds()); //distance = time * speed

	//NOTE: Moved to update in playerRobot && EnemyRobot
	//update the facing direction
	//sf::Vector2f newPosition = getPosition();
	//if(oldPosition.x != newPosition.x)
		//mFacingDirection = newPosition.x > oldPosition.x ? Right : Left;
}