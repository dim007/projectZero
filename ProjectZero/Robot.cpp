#include "stdafx.h"
#include "Robot.h"
#include "DataTables.h"
#include "utilities.h"
#include <cmath>
#include <queue>
#include <iostream>


using namespace std;

namespace { //anonymous namespace to avoid collisions for global function
	//used to initialize global Table
	const std::vector<RobotData> Table = initializeRobotData(); //found in DataTables.h
}
Robot::Robot(Robot::RobotType type, const TextureHolder& textures, const FontHolder& fonts)
	: Entity(Table[type].hitpoints),
	mType(type),
	mHealthDisplay(nullptr),
	mMissileDisplay(nullptr),
	mLivesDisplay(nullptr),
	mMissileAmmo(2),
	mFireCommand(),
	mMissileCommand(),
	mFireCountdown(sf::Time::Zero),
	mIsFiring(false),
	mIsLaunchingMissile(false),
	mIsMarkedForRemoval(false),
	mFireRateLevel(1),
	mSpreadLevel(1),
	mDropPickupCommand(),
	mTraveledDistance(0.f),
	mDirectionIndex(0),
	flightAbility(Table[type].flight),
	sign(1)
{
	mTextureID = getTexture();
	mHitBox.setTexture(textures.get(Table[type].texture), true);
	mHitBox.setTextureRect(Table[mType].hitBox);
	centerOrigin(mHitBox); //global helper method in utilities file

	//set command actions, for pushing to command queue
	setupCommands(textures);

	//for health display
	std::unique_ptr<TextNode> healthDisplay(new TextNode(fonts, ""));
	mHealthDisplay = healthDisplay.get(); //assigns the pointer to our local variable, unique means only 1 owner
	attachChild(std::move(healthDisplay)); //adds to the child list, for rendering
	
	updateTexts();
}
void Robot::drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const { //const { removed to allow changing of variables

	target.draw(mHitBox, states); //window.draw(sprite, location);
}
void Robot::updateCurrent(sf::Time dt, std::queue<Command>& commands) {

	//if destroyed check if droppped pickup power
	if (isDestroyed()) {
		checkPickupDrop(commands); //1/3 chance to spawn pickup
		mIsMarkedForRemoval = true;
		return;
	}
	//check if bullet or missle is being fired
	checkProjectileLaunch(dt, commands);

	//update enemy movement patterns, and apply velocity
	if (getCategory() == Category::EnemyRobot)
		setVelocity(0.f, 0.f); //must reset velocity for collision to work, Player velocity is reset in World.update
	updateMovementPattern(dt);
	if (!flightAbility)
		accelerate(0.f, mGravity);

	Entity::updateCurrent(dt, commands); //NOTE: All movement modifiers must be before updateCurr for the entity
	
	//set sign, facing right = 1, facing left = -1
	//sign = mFacingDirection == Facing::Right ? 1.f : -1.f;

	updateTexts();
}
Textures::ID Robot::getTexture() {
	switch (mType)
	{
	case Robot::Zero:
		return Textures::Animations;
	default:
		return Textures::Enemies;
	}
}
unsigned int Robot::getCategory() const {
	return Category::EnemyRobot;
}

sf::FloatRect Robot::getBoundingRect() const {
	return getWorldTransform().transformRect(mHitBox.getGlobalBounds());
}

bool Robot::isMarkedForRemoval() const {
	return mIsMarkedForRemoval;
}

bool Robot::isAllied() const {
	return mType == Robot::Zero;
}

float Robot::getMaxSpeed() const {
	return Table[mType].speed;
}

void Robot::collectMissiles(unsigned int count) {
	mMissileAmmo += count;
}
void Robot::fire() {
	if (Table[mType].fireInterval != sf::Time::Zero)
		mIsFiring = true;
}

void Robot::launchMissile() {
	if (mMissileAmmo > 0) {
		mIsLaunchingMissile = true;
		--mMissileAmmo;
	}
}

void Robot::updateTexts() {
	mHealthDisplay->setString(std::to_string(getHitpoints()) + "HP");
	mHealthDisplay->setPosition(0.f, 50.f);
	mHealthDisplay->setRotation(-getRotation());

	if (mMissileDisplay) {
		if (mMissileAmmo == 0)
			mMissileDisplay->setString("");
		else
			mMissileDisplay->setString("M: " + std::to_string(mMissileAmmo));
		mLivesDisplay->setString("Lives: " + std::to_string(getLives()));
	}
}
void Robot::updateMovementPattern(sf::Time dt) {
	const std::vector<Direction>& directions = Table[mType].directions;
	if (!directions.empty()) {
		float distanceToTravel = directions[mDirectionIndex].distance;
		if (mTraveledDistance > distanceToTravel) {
			mDirectionIndex = (mDirectionIndex + 1) % directions.size(); //mod makes it loop back to 0
			mTraveledDistance = 0.f;
		}
		float radians = toRadian(directions[mDirectionIndex].angle);
		float vx = getMaxSpeed() * std::cos(radians);
		float vy = getMaxSpeed() * std::sin(radians);
		setVelocity(vx, vy);
		mTraveledDistance += getMaxSpeed() * dt.asSeconds();
	}
}
void Robot::checkPickupDrop(std::queue<Command>& commands) {
	if (!isAllied() && randomInt(3) == 0)
		commands.push(mDropPickupCommand);
 }

void Robot::checkProjectileLaunch(sf::Time dt, std::queue<Command>& commands) {
	//enemies will always try to shoot
	if (!isAllied())
		fire(); //replaced in EnemyRobot, only shoot when player close

	//allow auto fire in intervals, after cool down
	if (mIsFiring && (mFireCountdown <= sf::Time::Zero)) { //limits shots to 4 per cooldown
		commands.push(mFireCommand);
	
		//reset interval
		if (fireCount >= Table[mType].fireAmount) { // num bullets for each interval
			mFireCountdown += Table[mType].fireInterval / (mFireRateLevel + 1.f);
			fireCount = 0;
		}
		else {
			fireCount++;
			mFireCountdown += sf::seconds(0.1f); 
		}
		mIsFiring = false;

	}
	else if (mFireCountdown > sf::Time::Zero) {
		mFireCountdown -= dt;
		mIsFiring = false;
	}

	if (mIsLaunchingMissile) {
		commands.push(mMissileCommand);
		mIsLaunchingMissile = false;
	}
}

void Robot::createBullets(SceneNode &node, const TextureHolder& textures) const {
	Projectile::Type type = isAllied() ? Projectile::AlliedBullet : Projectile::EnemyBullet;
	switch (mSpreadLevel)
	{
	case 1:
		//0.5 * size of sprite, puts it in the middle of sprite
		//0.33 is 1/3 the size of the sprite, etc
		createProjectile(node, type, 0.5f, 0.0f, textures);
		break;
	case 2:
		createProjectile(node, type, 0.33f, -0.33f, textures);
		createProjectile(node, type, 0.33f, +0.33f, textures);
		break;
	case 3:
		createProjectile(node, type, 0.33f, -0.5f, textures);
		createProjectile(node, type, 0.5f, 0.0f, textures);
		createProjectile(node, type, 0.5f, 0.33f, textures);
		break;
	default:
		break;
	}
}
//xyOffset is the ratio of where to spawn slash, 0.5 offset for middle of sprite
void Robot::createProjectile(SceneNode& node, Projectile::Type type, float xOffset, float yOffset, const TextureHolder& textures) const {
	std::unique_ptr<Projectile> projectile(new Projectile(type, textures));

	sf::Vector2f offset(xOffset * mHitBox.getGlobalBounds().width, yOffset * mHitBox.getGlobalBounds().height);
	sf::Vector2f velocity(projectile->getMaxSpeed(), 0);
	if (mType == RobotType::Drone) //fix drone to shoot downwards
		velocity.y = velocity.x;

	//Fix for "sign", enemies shoot opposite direction
	if (isAllied()) {
		// sign is pos for right, neg for left
		projectile->setPosition(getWorldPosition() + offset * sign);
		projectile->setVelocity(velocity * sign);
	}
	else { //enemies has inverted "sign" have to invert again to make shoot towards player
		projectile->setPosition(getWorldPosition() + offset * -sign);
		projectile->setVelocity(velocity.x * -sign, velocity.y);
	}

	float rotation = sign == 1.f ? 0.f : 180.f; //bullet sprite rotation 
	projectile->setRotation(rotation);
	node.attachChild(std::move(projectile));
}
void Robot::createPickup(SceneNode& node, const TextureHolder& textures) const {
	auto type = static_cast<Pickup::Type>(randomInt(Pickup::TypeCount));

	std::unique_ptr<Pickup> pickup(new Pickup(type, textures));
	pickup->setPosition(getWorldPosition());
	pickup->setVelocity(1.f, 0.f);
	node.attachChild(std::move(pickup));
}
void Robot::setGravity(const float gravity) {
	mGravity = gravity;
}


void Robot::setupCommands(const TextureHolder& textures) {
	mFireCommand.category = Category::ActionLayer; //has to be action layer, to attatch to SceneNode actionlayer
	mFireCommand.action = [this, &textures](SceneNode& node, sf::Time) {
		createBullets(node, textures);
	};

	mMissileCommand.category = Category::ActionLayer;
	mMissileCommand.action = [this, &textures](SceneNode& node, sf::Time) {
		createProjectile(node, Projectile::Missile, 0.5f, 0.f, textures);
	};

	mDropPickupCommand.category = Category::ActionLayer; //
	mDropPickupCommand.action = [this, &textures](SceneNode& node, sf::Time) {
		createPickup(node, textures);
	};
}