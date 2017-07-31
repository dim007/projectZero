#include "stdafx.h"
#include "EnemyRobot.h"
#include "utilities.h"
#include "DataTables.h"
#include <iostream>
using namespace std;

namespace { //anonymous namespace to avoid collisions for global function
			//used to initialize global Table
	const std::vector<RobotData> Table = initializeRobotData(); //found in DataTables.h
}
EnemyRobot::EnemyRobot(const RobotType type, const TextureHolder& textures, const FontHolder& fonts, sf::Vector2f& playerPosition) 
	: Robot(type, textures, fonts),
	isRunning(false),
	isShooting(false),
	isAttacking(false),
	mType(type)
{
	mPlayerPosition = &playerPosition;
	setHitpoints(Table[mType].hitpoints);
	mHitBox.setTexture(textures.get(Table[mType].texture));
	mHitBox.setTextureRect(Table[mType].hitBox);
	centerOrigin(mHitBox);

	setAnimations(textures);
	setupCommands(textures);

	updateTexts();
}
unsigned int EnemyRobot::getCategory() const {
	return Category::EnemyRobot;
}
sf::FloatRect EnemyRobot::getBoundingRect() const {
	return getWorldTransform().transformRect(mHitBox.getGlobalBounds());
}

void EnemyRobot::drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const {
	if (isAttacking && Table[mType].hasAttackAnim)
		target.draw(attackAnimation, states);
	else if (isShooting && Table[mType].hasShootAnim)
		target.draw(shootAnimation, states);
	else 
		target.draw(runAnimation, states);
}
void EnemyRobot::handleAnimationUpdate(sf::Time dt) {
	if (isAttacking && Table[mType].hasAttackAnim)
		attackAnimation.update(dt);
	else if (isShooting && Table[mType].hasShootAnim)
		shootAnimation.update(dt);
	else 
		runAnimation.update(dt);

	if (!isAttacking && Table[mType].hasAttackAnim)
		attackAnimation.reset();
	if (!isShooting && Table[mType].hasShootAnim)
		shootAnimation.reset();
}
void EnemyRobot::updateCurrent(sf::Time dt, std::queue<Command>& commands) {
	//if destroyed check if droppped pickup power
	if (isDestroyed()) {
		checkPickupDrop(commands); //1/3 chance to spawn pickup
		mIsMarkedForRemoval = true;
		return;
	}
	oldPosition = getWorldPosition();

	if (getCategory() == Category::EnemyRobot)
		setVelocity(0.f, 0.f); //must reset velocity for collision to work, Player velocity is reset in World.update
	
	//handle animation choices
	handleAnimationUpdate(dt);

	//check if bullet or missle is being fired
	checkAttack(dt, commands);
	checkProjectileLaunch(dt, commands);
	//Replaced movement parterns with following the player and attacking
	//updateMovementPattern(dt);
	handleMovement(dt);

	if (!flightAbility && !isGrounded) //add gravity
		accelerate(0.f, mGravity);
	if (isAttacking) //stop moving when slashing
		setVelocity(0.f, 0.f);//getVelocity().y);

	Entity::updateCurrent(dt, commands); //NOTE: All movement modifiers must be before updateCurr for the entity
	//NOTE : SIGN has to change sign value for enemies, Right is now -1.f
	sign = (mPlayerPosition->x < getWorldPosition().x) ? 1.f : -1.f;

														   //determine if running/falling
	newPosition = getWorldPosition();
	isRunning = isGrounded && !isHanging && oldPosition.x != newPosition.x ? true : false;

	isGrounded = false;
	isHanging = false;
	updateTexts();
}
void EnemyRobot::handleMovement(sf::Time dt) {

	//move Robot closer
	if (movementCooldown > sf::Time::Zero) {
		movementCooldown -= dt;
		return;
	}
	//if not in range do nothing
	if (std::abs(mPlayerPosition->x - getWorldPosition().x) > 300.f)
		return;

	//if in range and charge at player
	if (std::abs(mPlayerPosition->x - getWorldPosition().x) > Table[mType].attackDistanceWidth) {
		if (mPlayerPosition->x - getWorldPosition().x < 0) {//move left 
			accelerate(-Table[mType].speed, 0.f);
		}
		else {
			accelerate(Table[mType].speed, 0.f); //else right
		}
	}
	//if in range move closer to player
	if (Table[mType].flight && std::abs(mPlayerPosition->y - getWorldPosition().y) > Table[mType].attackDistanceHeight) {
		//If enemy is higher than player
		if (mPlayerPosition->y - getWorldPosition().y > Table[mType].attackDistanceHeight) {
			accelerate(0.f, Table[mType].speed); //go down
		}
		//if enemy close to player
		else {
			accelerate(0.f, -Table[mType].speed); //move up
		}
	}
	movementAllowance += dt;
	if (movementAllowance >= Table[mType].movementAllowance) {
		movementCooldown = Table[mType].movementCooldown;
		movementAllowance = sf::Time::Zero;
	}
}
void EnemyRobot::checkProjectileLaunch(sf::Time dt, std::queue<Command>& commands) {
	if (std::abs(mPlayerPosition->x - getWorldPosition().x) <= Table[mType].attackDistanceWidth)
		fire(); //only shoot when player close

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
}

void EnemyRobot::checkAttack(sf::Time dt, std::queue<Command>& commands) {
	if (!Table[mType].hasAttackAnim) //check to see if creature has attack feature
		return;
	//attack on specific frame of attack animation
	if (isAttacking && attackAnimation.mCurrentFrame == Table[mType].attackFrame)
		commands.push(attackCommand);
	//update atttackAnimation until completion
	if (isAttacking && attackAllowance < Table[mType].attackDuration) {
		attackAllowance += dt;
		if (attackAllowance > Table[mType].attackDuration)
			isAttacking = false;
		return;
	}
	//cooldown
	if (attackCooldown >= sf::Time::Zero || !Table[mType].hasAttackAnim) {
		attackCooldown -= dt;
		return;
	}
	//if in range, attack
	if (std::abs(mPlayerPosition->x - getWorldPosition().x) <= Table[mType].attackDistanceWidth) {
		attackCooldown = Table[mType].attackCooldown;
		isAttacking = true;
		attackAllowance = sf::Time::Zero;
	}
}
void EnemyRobot::createAttack(SceneNode& node, Attack::Type type, float xOffset, float yOffset, const TextureHolder& textures) {
	std::unique_ptr<Attack> attack(new Attack(type, textures, sign));
	sf::Vector2f offset(xOffset * mHitBox.getGlobalBounds().width, yOffset * mHitBox.getGlobalBounds().height);
	//"sign" inverted, to be directed at player
	attack->setPosition(sf::Vector2f(getWorldPosition().x + offset.x * -sign, getWorldPosition().y + offset.y));
	node.attachChild(std::move(attack));
}
void EnemyRobot::setAnimations(const TextureHolder& textures) {
	runAnimation.setTexture(textures.get(Table[mType].texture));
	runAnimation.setVectorFrames(Table[mType].runFramesVector);
	runAnimation.setNumFrames(Table[mType].runNumFrames);
	runAnimation.setDuration(Table[mType].runDuration);
	runAnimation.setRepeating(true);
	runAnimation.setSign(sign);
	runAnimation.update(sf::seconds(0.01f));

	if (Table[mType].hasAttackAnim) {
		attackAnimation.setTexture(textures.get(Table[mType].texture));
		attackAnimation.setVectorFrames(Table[mType].attackFramesVector);
		attackAnimation.setNumFrames(Table[mType].attackNumFrames);
		attackAnimation.setDuration(Table[mType].attackDuration);
		attackAnimation.setSign(sign);
		//attackAnimation.update(sf::seconds(0.01f));
	}
	if (Table[mType].hasShootAnim) {
		shootAnimation.setTexture(textures.get(Table[mType].texture));
		shootAnimation.setVectorFrames(Table[mType].shootFramesVector);
		shootAnimation.setNumFrames(Table[mType].shootNumFrames);
		shootAnimation.setDuration(Table[mType].shootDuration);
		shootAnimation.setSign(sign);
		shootAnimation.update(sf::seconds(0.01f));
	}
}

void EnemyRobot::setupCommands(const TextureHolder& textures) {
	attackCommand.category = Category::ActionLayer; //has to be action layer to be drawn on the platformLayer
	//the attack itself has its own category, this command is for action layer only tho
	attackCommand.action = [this, &textures](SceneNode& node, sf::Time) {
		//attackAnimation.reset(); //removed bc attack is now on specific frame
		createAttack(node, Table[mType].attack, 0.4f, 0.f, textures);
	};
	shootCommand.category = Category::ActionLayer;
	shootCommand.action = [this, &textures](SceneNode& node, sf::Time) {
		createProjectile(node, Table[mType].projectile, 0.5f, 0.f, textures);
	};
}