#ifndef ENTITY
#define ENTITY
#include "stdafx.h"
#include "SceneNode.h"

class Entity : public SceneNode{
public:
	//Facing mFacingDirection;

	explicit Entity(int hitpoints);
	void repair(int points);
	void damage(int points);
	void destroy();
	
	int getHitpoints() const;
	int mLives;
	int getLives();
	void setLives(int lives);
	void setHitpoints(int hits);
	bool isDestroyed() const;

	void setVelocity(sf::Vector2f velocity);
	void setVelocity(float vx, float vy);
	void accelerate(sf::Vector2f velocity);
	void accelerate(float vx, float vy);
	sf::Vector2f getVelocity() const;
private:
	int mHitpoints;
	sf::Vector2f mVelocity;
protected:
	virtual void updateCurrent(sf::Time dt, std::queue<Command> commands);
};

#endif