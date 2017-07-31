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
	sliding(false),
	isGrounded(false),
	isJumping(false),
	isWallJump(false),
	jumpCount(0),
	mWallJumpCountdown(sf::Time::Zero),
	mFacing(Facing::Right)
{
	mTextureID = getTexture();
	mSprite.setTexture(textures.get(Table[type].texture), true);
	centerOrigin(mSprite); //global helper method in utilities file
	
	//set command actions, for pushing to command queue
	setupCommands(textures);

	//for health display
	std::unique_ptr<TextNode> healthDisplay(new TextNode(fonts, ""));
	mHealthDisplay = healthDisplay.get(); //assigns the pointer to our local variable, unique means only 1 owner
	attachChild(std::move(healthDisplay)); //adds to the child list, for rendering
	
	//for missile display
	if (getCategory() == Category::PlayerRobot) {
		std::unique_ptr<TextNode> missileDisplay(new TextNode(fonts, ""));
		missileDisplay->setPosition(0, 70);
		mMissileDisplay = missileDisplay.get();
		attachChild(std::move(missileDisplay));
	}
	updateTexts();
}
void Robot::drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const {
	target.draw(mSprite, states); //window.draw(sprite, location);
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
	//checkAttack(dt, commands);

	//update enemy movement patterns, and apply velocity
	if (getCategory() == Category::EnemyRobot)
		setVelocity(0.f, 0.f); //must reset velocity for collision to work, Player velocity is reset in World.update
	updateMovementPattern(dt);

	//*****Jump********
	checkJump(commands);
	if (mWallJumpCountdown > sf::Time::Zero)
		mWallJumpCountdown -= dt;
	if (!flightAbility && !sliding)
		accelerate(0.f, mGravity);
	else if (!flightAbility && sliding)
		setVelocity(getVelocity().x, 0.f);
	//*****Jump end*****
	Entity::updateCurrent(dt, commands); //NOTE: All movement modifiers must be before updateCurr for the entity

	sliding = false;
	updateTexts();
}
Textures::ID Robot::getTexture() {
	switch (mType)
	{
	case Robot::Zero:
		return Textures::Zero;
	case Robot::X:
		return Textures::X;
	case Robot::Drone:
		return Textures::Drone;
	default:
		return Textures::Drone;
		break;
	}
}
unsigned int Robot::getCategory() const {
	switch (mType) {
	case Robot::Zero:
		return Category::PlayerRobot; //found in command.h
	default:
		return Category::EnemyRobot;
	}
}

sf::FloatRect Robot::getBoundingRect() const {
	return getWorldTransform().transformRect(mSprite.getGlobalBounds());
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

void Robot::increaseFireRate() {
	if (mFireRateLevel < 10)
		++mFireRateLevel;
}
void Robot::increaseSpread() {
	if (mSpreadLevel < 3)
		mSpreadLevel++;
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
/*
void Robot::checkAttack(sf::Time dt, std::queue<Command>& commands) {


	if (attackWait >= sf::Time::Zero) {//reduct time wait
		attackWait -= dt;
		return;
	}

	if (attackCount <= 0 && attackWait <= sf::Time::Zero || attackCount <= 0) { //basecase
		slashing1 = slashing2 = slashing3 = false; //reset slashing when not in use
		return;
	}

	if (attackCount >= 1 && slashing1 == slashing2 == slashing3 == false) {
		commands.push(slash1Command);
		slashing1 = true;
		attackWait += sf::seconds(0.5);
		attackCount--;
		return;
	}
	if (attackCount >= 1 && slashing1 == true) {
		commands.push(slash2Command);
		slashing2 = true;
		slashing1 = false;
		attackWait += sf::seconds(0.5);
		attackCount--;
		return;
	}
	if (attackCount >= 1 && slashing2 == true) {
		commands.push(slash3Command);
		slashing3 = true;
		slashing2 = false;
		attackWait += sf::seconds(0.5);
		attackCount = 0; //end the combo
		return;
	}

}*/
void Robot::checkProjectileLaunch(sf::Time dt, std::queue<Command>& commands) {
	//enemies will always try to shoot
	if (!isAllied())
		fire();

	//allow auto fire in intervals, after cool down
	if (mIsFiring && mFireCountdown <= sf::Time::Zero) {
		commands.push(mFireCommand);
		//reset interval
		mFireCountdown += Table[mType].fireInterval / (mFireRateLevel + 1.f);
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
void Robot::createProjectile(SceneNode& node, Projectile::Type type, float xOffset, float yOffset, const TextureHolder& textures) const {
	std::unique_ptr<Projectile> projectile(new Projectile(type, textures));

	sf::Vector2f offset(xOffset * mSprite.getGlobalBounds().width, yOffset * mSprite.getGlobalBounds().height);
	sf::Vector2f velocity(projectile->getMaxSpeed(), 0);

	float sign = mFacing == Facing::Right ? 1.f : -1.f; //direction of missile, pos for right
	projectile->setPosition(getWorldPosition() + offset * sign);
	projectile->setVelocity(velocity * sign);
	float rotation = mFacing == Facing::Right ? 0.f : 180.f;
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
void Robot::jump() {
	isJumping = false;
	if (isGrounded)
		jumpCount = 0;

	if (!sliding && jumpCount <= 14) {
		isJumping = true;
		jumpCount++;
	}

	else if (sliding && mWallJumpCountdown <= sf::Time::Zero) {
		isJumping = true;
		jumpCount = 3;
		mWallJumpCountdown += sf::seconds(0.5);
	}
}
void Robot::checkJump(std::queue<Command>& commands) {
	if (!isJumping)
		return;
	if (isJumping && !sliding) {
		commands.push(mJumpCommand);
		isGrounded = false;
		isJumping = false;
	}
	else if (isJumping && sliding) {
		for(int x = 0; x < jumpCount ; x++)
			commands.push(mWallJumpCommand);
		isJumping = false;
	}
}/*
void Robot::dash() {
	float sign = mFacing == Facing::Right ? 1.f : -1.f;
	const float dashSpeed = 200.f;

	if (!dashing) {
		dashing = true;
		accelerate(dashSpeed * sign, 0.f);
		dashCount++;
	}
	else if (dashing && dashCount <= 10) {
		dashCount++;
		accelerate(dashSpeed * sign, 0.f);
	}
	if (dashCount >= 10) {
		stopDash();
	}
		
}
void Robot::stopDash() {
	dashing = false;
	dashCount = 0;
}
void Robot::slash() {
	attackCount++;
}

//xyOffset is the ratio of where to spawn slash, 0.5 offset for middle of sprite
void Robot::createAttack(SceneNode& node, Attack::Type type, float xOffset, float yOffset, const TextureHolder& textures) const {
	std::unique_ptr<Attack> attack(new Attack(type, textures));

	sf::Vector2f offset(xOffset * mSprite.getGlobalBounds().width, yOffset * mSprite.getGlobalBounds().height);
	float direction = mFacing == Facing::Right ? 1.f : -1.f;
	attack->setPosition(getWorldPosition() + offset * direction);
	node.attachChild(std::move(attack));
}
*/
void Robot::setupCommands(const TextureHolder& textures) {
	mJumpCommand.category = Category::PlayerRobot;
	mJumpCommand.action = [this](SceneNode& node, sf::Time) {
		accelerate(0.f, -750.f);
	};
	/*
	slash1Command.category = Category::PlayerRobot;
	slash1Command.action = [this, &textures](SceneNode& node, sf::Time) {
		createAttack(node, Attack::Slash1, 0.5, 0.f, textures);
	};
	slash2Command.category = Category::PlayerRobot;
	slash2Command.action = [this, &textures](SceneNode& node, sf::Time) {
		createAttack(node, Attack::Slash2, 0.5, 0.f, textures);
	};
	slash3Command.category = Category::PlayerRobot;
	slash3Command.action = [this, &textures](SceneNode& node, sf::Time) {
		createAttack(node, Attack::Slash3, 0.5, 0.f, textures);
	};
	*/
	mWallJumpCommand.category = Category::PlayerRobot;
	mWallJumpCommand.action = [this](SceneNode& node, sf::Time) {
		accelerate(900.f, -500.f);
	};

	mFireCommand.category = Category::ActionLayer; //has to be action layer bc of buildScene in World is also ActionLayer
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