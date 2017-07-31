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
		X,
		Drone,
		TypeCount
	};
	enum Facing {
		Left,
		Right
	};
	//easier to make it use RobotType as key, beats having two enums for textures and types
	typedef ResourceHolder<sf::Texture, Textures::ID> TextureHolder; //shortcut for typing
	typedef ResourceHolder<sf::Font, Fonts::ID> FontHolder;


public:
	explicit Robot(Robot::RobotType type, const TextureHolder& textures, const FontHolder& fonts); //explicit is for constructor
	Textures::ID getTexture();

	void setGravity(const float gravity);
	bool sliding; //for wall friction
	bool isGrounded, isJumping, isWallJump;
	void jump();
	void dash();
	void stopDash();
	void slash();

	virtual unsigned int getCategory() const;
	virtual sf::FloatRect getBoundingRect() const;
	virtual bool isMarkedForRemoval() const;
	bool isAllied() const;
	float getMaxSpeed() const;

	//missle
	void increaseFireRate();
	void increaseSpread();
	void collectMissiles(unsigned int count);
	void fire();
	void launchMissile();

private: 
	virtual void drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const;
	void updateCurrent(sf::Time dt, std::queue<Command>& commands);
	void updateTexts();
	void updateMovementPattern(sf::Time dt);
	void setupCommands(const TextureHolder& textures);

	void checkAttack(sf::Time dt, std::queue<Command>& commands);
	void checkProjectileLaunch(sf::Time dt, std::queue<Command>& commands);
	void checkPickupDrop(std::queue<Command>& commands);
	void checkJump(std::queue<Command>& commands);

	void createAttack(SceneNode& node, Attack::Type type, float xOffset, float yOffset, const TextureHolder& textures) const;
	void createBullets(SceneNode& node, const TextureHolder& textures) const;
	void createProjectile(SceneNode& node, Projectile::Type type, float xOffset, float yOffset, const TextureHolder& textures) const;
	void createPickup(SceneNode& node, const TextureHolder& textures) const;

private:
	Textures::ID mTextureID;
	RobotType mType;
	sf::Sprite mSprite;

	TextNode* mHealthDisplay;
	TextNode* mMissileDisplay;

	Command mFireCommand;
	Command mMissileCommand;
	Command mDropPickupCommand;
	Command mJumpCommand;
	Command mWallJumpCommand;
	Command slash1Command, slash2Command, slash3Command;
	sf::Time mFireCountdown;
	sf::Time mWallJumpCountdown;
	bool mIsFiring;
	bool mIsLaunchingMissile;
	bool mIsMarkedForRemoval;

	int jumpCount;
	int dashCount;
	bool dashing;
	int mFireRateLevel;
	int mSpreadLevel;
	int mMissileAmmo;
	bool flightAbility;
	float mGravity;

	int attackCount;
	sf::Time attackWait;
	bool slashing1, slashing2, slashing3;

	Facing mFacing;
	float mTraveledDistance;
	std::size_t mDirectionIndex;

};
#endif