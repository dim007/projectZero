#ifndef ROBOT
#define ROBOT
#include "stdafx.h"
#include "Entity.h"
#include "ResourceHolder.h"
#include "SFML\Graphics.hpp"
#include "TextureID.h" // Needed for Textures ID
#include "TextNode.h"
#include <queue>
#include "Projectile.h"
#include "Pickup.h"
#include "Attack.h"

class Robot : public Entity {
public:
	enum RobotType { //refrenced as Robot:: not RobotType::
		Zero,
		slashSuit,
		flySuit,
		Drone,
		Dog,
		TypeCount
	};
	//easier to make it use RobotType as key, beats having two enums for textures and types
	typedef ResourceHolder<sf::Texture, Textures::ID> TextureHolder; //shortcut for typing
	typedef ResourceHolder<sf::Font, Fonts::ID> FontHolder;


public:
	explicit Robot(Robot::RobotType type, const TextureHolder& textures, const FontHolder& fonts); //explicit is for constructor
	Textures::ID getTexture();

	void setGravity(const float gravity);
	bool flightAbility, isGrounded, isHanging;

	virtual unsigned int getCategory() const;
	virtual sf::FloatRect getBoundingRect() const;
	virtual bool isMarkedForRemoval() const;
	bool isAllied() const;
	float getMaxSpeed() const;

	//missle
	void collectMissiles(unsigned int count);
	void fire();
	void launchMissile();

protected: 
	virtual void drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const;
	void updateCurrent(sf::Time dt, std::queue<Command>& commands);
	void updateTexts();
	void updateMovementPattern(sf::Time dt);
	void setupCommands(const TextureHolder& textures);

	void checkProjectileLaunch(sf::Time dt, std::queue<Command>& commands);
	void checkPickupDrop(std::queue<Command>& commands);

	void createBullets(SceneNode& node, const TextureHolder& textures) const;
	void createProjectile(SceneNode& node, Projectile::Type type, float xOffset, float yOffset, const TextureHolder& textures) const;
	void createPickup(SceneNode& node, const TextureHolder& textures) const;

protected:
	Textures::ID mTextureID;
	RobotType mType;
	sf::Sprite mHitBox;

	TextNode* mHealthDisplay;
	TextNode* mMissileDisplay;
	TextNode* mLivesDisplay;

	Command mFireCommand;
	Command mMissileCommand;
	Command mDropPickupCommand;

	sf::Time mFireCountdown;
	bool mIsFiring;
	int fireCount;
	bool mIsLaunchingMissile;
	bool mIsMarkedForRemoval;

	int mFireRateLevel;
	int mSpreadLevel;
	int mMissileAmmo;
	float mGravity;
	float sign;

	float mTraveledDistance;
	std::size_t mDirectionIndex;

	//for run animation
	sf::Vector2f oldPosition, newPosition;

};
#endif