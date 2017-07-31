#include "stdafx.h"
#include "playerRobot.h"
#include <vector>
#include "utilities.h"
#include <iostream>
using namespace std;

namespace { //anonymous namespace to avoid collisions for global function
			//used to initialize global Table
	const std::vector<RobotData> Table = initializeRobotData(); //found in DataTables.h
}
playerRobot::playerRobot(Robot::RobotType type, const TextureHolder& textures, const FontHolder& fonts)
	: Robot(type, textures, fonts),
	mType(type),
	isJumping(false),
	isHangJump(false),
	slashing1(false),
	slashing2(false),
	stopSlash(false),
	jumpTime(sf::Time::Zero),
	deathTime(sf::Time::Zero),
	jumpCooldown(sf::Time::Zero),
	lifeTime(sf::Time::Zero)
{
	setHitpoints(Table[mType].hitpoints);
	mTextureID = getTexture();
	mHitBox.setTexture(textures.get(Textures::Animations));
	mHitBox.setTextureRect(sf::IntRect(0, 0, 40, 40));
	centerOrigin(mHitBox); //global helper method in utilities file

	//setAnimations
	setAnimations(textures);

	//set command actions, for pushing to command queue
	setupCommands(textures);

	//for missile display, healthdisplay in Robot constructor
	std::unique_ptr<TextNode> missileDisplay(new TextNode(fonts, ""));
	missileDisplay->setPosition(0, 70);
	mMissileDisplay = missileDisplay.get();
	attachChild(std::move(missileDisplay));

	std::unique_ptr<TextNode> livesDisplay(new TextNode(fonts, ""));
	livesDisplay->setPosition(0, 90);
	mLivesDisplay = livesDisplay.get();
	attachChild(std::move(livesDisplay));

	updateTexts();
	isSpawning = true;
}

void playerRobot::drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const {
	if (isSpawning)
		target.draw(spawnAnimation, states);
	else if (isDestroyed())
		target.draw(deathAnimation, states);
	else if (isTakingDamage)
		target.draw(takeDamage2Animation, states);
	else if (jumpSlash && (isJumping || isFalling))
		target.draw(jumpSlashAnimation, states);
	else if (isFalling)
		target.draw(fallAnimation, states);
	else if (canDash && (isDashing || airDashing))
		target.draw(dashAnimation, states);
	else if (hangSlash && isHanging)
		target.draw(hangSlashAnimation, states);
	else if (slashing1)
		target.draw(slash1Animation, states);
	else if (slashing2)
		target.draw(slash2Animation, states);
	else if (slashing3)
		target.draw(slash3Animation, states);
	else if (isJumping && !isHangJump)
		target.draw(jumpAnimation, states);
	else if (isJumping && isHangJump)
		target.draw(hangJumpAnimation, states);
	else if (isHanging) 
		target.draw(hangAnimation, states);
	else if (isRunning)
		target.draw(runAnimation, states);
	else
		target.draw(standAnimation, states); //window.draw(sprite, location);
}
sf::FloatRect playerRobot::getBoundingRect() const {
	return getWorldTransform().transformRect(mHitBox.getGlobalBounds());
}
void playerRobot::updateCurrent(sf::Time dt, std::queue<Command>& commands) {

	oldPosition = getWorldPosition();

	//handle animation choices
	handleAnimationUpdate(dt);

	checkTakeDamage(dt);
	if (!isTakingDamage) {
		//check if bullet or missle is being fired
		checkProjectileLaunch(dt, commands);
		checkAttack(dt, commands);

		//check jump, add gravity
		checkJump(dt);
		checkDash(dt);

		if (!flightAbility && !isHanging) //add gravity
			accelerate(0.f, mGravity);
		if (!flightAbility && (isHanging || airDashing)) //defy gravity
			setVelocity(getVelocity().x, 0.f);
		if (slashing1 || slashing2 || slashing3) //stop moving when slashing
			setVelocity(0.f, getVelocity().y);
	}
	Entity::updateCurrent(dt, commands); //NOTE: All movement modifiers must be before updateCurr for the entity
		
	if (!isTakingDamage) {
		//determine if running/falling
		newPosition = getWorldPosition();
		if(newPosition.x != oldPosition.x)
			sign = (newPosition.x > oldPosition.x) ? 1.f : -1.f; //set sign, facing right = 1, facing left = -1
		isRunning = isGrounded && !isDashing && !isHanging && oldPosition.x != newPosition.x ? true : false;
		isFalling = !isGrounded && !isDashing && !isHanging && oldPosition.y <= newPosition.y ? true : false;
	}
	if (isGrounded) {
		airDashing = false;
		canDash = true;
		isJumping = false;
		isHangJump = false;
		isFalling = false;
		jumpSlash = false;
	}

	isGrounded = false;
	isHanging = false;
	updateTexts();
}
unsigned int playerRobot::getCategory() const {
	return Category::PlayerRobot;
}
void playerRobot::checkTakeDamage(sf::Time dt) {
	if (!isTakingDamage)
		return;
	takeDamageCountDown += dt;
	accelerate(200.f * damageDirection, -100.f); // this launches you backwards(no matter facing right/left) and upwards
	if (takeDamageCountDown >= takeDamageAnimation.getDuration()) {
		takeDamageCountDown = sf::Time::Zero;
		isTakingDamage = false;
	}
}
void playerRobot::checkAttack(sf::Time dt, std::queue<Command>& commands) {

	if (attackCountdown >= sf::Time::Zero) {//reduct time wait
		attackCountdown -= dt;
		return;
	}

	jumpSlashAnimation.reset();
	if (attackCount <= 0) { //base case
		slashing1 = false; //reset slashing when not in use
		slashing2 = false;
		slashing3 = false;
		hangSlash = false;
		jumpSlash = false;
	}
	if (attackCount >= 1 && isHanging) {
		commands.push(hangSlashCommand);
		hangSlash = true;
		attackCountdown += sf::seconds(0.4f);
		attackCount = 0;
		return;
	}
	if (attackCount >= 1 && !isGrounded && (isJumping || isFalling)) {
		commands.push(jumpSlashCommand);
		jumpSlash = true;
		attackCountdown += sf::seconds(0.4f);
		attackCount = 0;
		return;
	}
	if (attackCount >= 1 && slashing1 == false && slashing2 == false && slashing3 == false && isGrounded) {
		commands.push(slash1Command);
		slashing1 = true;
		attackCountdown += sf::seconds(0.3f);
		attackCount--;
		return;
	}
	if (attackCount >= 1 && slashing1 == true && isGrounded) {
		commands.push(slash2Command);
		slashing2 = true;
		slashing1 = false;
		slash1Animation.reset();
		attackCountdown += sf::seconds(0.3f);
		attackCount--;
		return;
	}
	if (attackCount >= 1 && slashing2 == true && isGrounded) {
		commands.push(slash3Command);
		slashing2 = false;
		slash2Animation.reset();
		slashing3 = true;
		attackCountdown += sf::seconds(0.4f);
		attackCount = 0; //end the combo
		return;
	}
}
//xyOffset is the ratio of where to spawn slash, 0.5 offset for middle of sprite
void playerRobot::createAttack(SceneNode& node, Attack::Type type, float xOffset, float yOffset, const TextureHolder& textures) {
	std::unique_ptr<Attack> attack(new Attack(type, textures, sign));
	sf::Vector2f offset(xOffset * mHitBox.getGlobalBounds().width, yOffset * mHitBox.getGlobalBounds().height);

	attack->setPosition(sf::Vector2f(getWorldPosition().x + offset.x * sign, getWorldPosition().y + offset.y));
	if(type == Attack::hangSlash)
		attack->setPosition(sf::Vector2f(getWorldPosition().x + offset.x * -sign, getWorldPosition().y + offset.y));

	node.attachChild(std::move(attack));
}
void playerRobot::checkJump(sf::Time dt) {

	if (isGrounded || isHanging) {
		jumpTime = sf::Time::Zero;
		jumpAnimation.reset();
		fallAnimation.reset();
		hangJumpAnimation.reset();
		isHangJump = false;
		if (jumpCooldown >= sf::Time::Zero)
			jumpCooldown -= dt;
	}
	if (jumpCooldown > sf::Time::Zero || !isJumping)
		return;

	if (isJumping && !isHanging && jumpTime <= sf::seconds(0.35f)) {
		isGrounded = false;
		accelerate(0.f, -750.f + 10.f * jumpTime.asSeconds()); //Reduces jump speed relative to height
		jumpTime += dt;
		if (isHangJump && jumpTime >= sf::seconds(0.15f)) {
			jumpCooldown += sf::seconds(0.5f);
			isJumping = false;
		}
		if (jumpTime >= sf::seconds(0.35f)) {
			jumpCooldown += sf::seconds(0.1f);
			isJumping = false;
		}
	}
	//handle wall jumps first here and then finish up at the top
	else if (isJumping && jumpTime <= sf::seconds(0.15f)) {
		isHangJump = true;
		isHanging = false;
		isGrounded = false;
		accelerate(2000.f * -sign, -1000.f);
		jumpTime += dt;
	}
}
void playerRobot::checkDash(sf::Time dt) {
	if (!isDashing) {
		dashTime = sf::Time::Zero;
		dashAnimation.reset();
		return;
	}

	const float dashSpeed = 400.f;

	if (!isGrounded && dashTime <=sf::Time::Zero && canDash && !airDashing) {
		airDashing = true;
	}

	if (dashTime <= sf::seconds(0.4f) && canDash) //canDash prevents 2nd air dash
		accelerate(dashSpeed * sign, 0.f);
	if (dashTime >= sf::seconds(0.4f) && !airDashing && !isGrounded && isDashing) { //makes jump dash possible
		dashTime = sf::seconds(0.3f);
		accelerate(0.f, 100.f);
	}
	if (dashTime >= sf::seconds(0.4f)) {
		isDashing = false;
		airDashing = false;
		canDash = false;
		dashTime = sf::Time::Zero;
		if (!isGrounded)
			canDash = false;
		dashAnimation.reset();
	}
	dashTime += dt;

}
void playerRobot::setupCommands(const TextureHolder& textures) {
	slash1Command.category = Category::ActionLayer; //Has to be action layer for attack to show
	slash1Command.action = [this, &textures](SceneNode& node, sf::Time) {
		slash1Animation.reset();
		createAttack(node, Attack::Slash1, 0.9f, -0.3f, textures);
	};
	slash2Command.category = Category::ActionLayer;
	slash2Command.action = [this, &textures](SceneNode& node, sf::Time) {
		slash2Animation.reset();
		createAttack(node, Attack::Slash2, 0.7f, 0.f, textures);
	};
	slash3Command.category = Category::ActionLayer;
	slash3Command.action = [this, &textures](SceneNode& node, sf::Time) {
		slash3Animation.reset();
		createAttack(node, Attack::Slash3, 1.1f, -0.25f, textures);
	};
	jumpSlashCommand.category = Category::ActionLayer;
	jumpSlashCommand.action = [this, &textures](SceneNode& node, sf::Time) {
		jumpSlashAnimation.reset();
		createAttack(node, Attack::jumpSlash, 0.6f, 0.4f, textures);
	};
	hangSlashCommand.category = Category::ActionLayer;
	hangSlashCommand.action = [this, &textures](SceneNode& node, sf::Time) {
		hangSlashAnimation.reset();
		createAttack(node, Attack::hangSlash, 1.f, -0.3f, textures);
	};
}

bool playerRobot::isMarkedForRemoval() const {
	return isDestroyed() && deathAnimation.getDuration() >= deathTime;
}
void playerRobot::handleAnimationUpdate(sf::Time dt) {
	//spawning
	if (isSpawning && lifeTime >= spawnAnimation.getDuration())
		isSpawning = false;
	if (isSpawning) {
		spawnAnimation.update(dt); //updates sprite image, using dt
		accelerate(0.f, 200.f);
	}
	else if (isDestroyed()) {
		deathAnimation.update(dt);
		deathTime += dt;
	}
	else if (isTakingDamage) {
		takeDamage2Animation.update(dt);
	}
	else if (jumpSlash && (isJumping || isFalling))
		jumpSlashAnimation.update(dt);
	else if (isFalling)
		fallAnimation.update(dt);
	else if (canDash && (isDashing || airDashing))
		dashAnimation.update(dt);
	else if (hangSlash && isHanging)
		hangSlashAnimation.update(dt);
	else if (slashing1)
		slash1Animation.update(dt);
	else if (slashing2)
		slash2Animation.update(dt);
	else if (slashing3)
		slash3Animation.update(dt);
	else if (isJumping && !isHangJump)
		jumpAnimation.update(dt);
	else if (isJumping && isHangJump)
		hangJumpAnimation.update(dt);
	else if (isHanging) 
		hangAnimation.update(dt);
	else if (isRunning)
		runAnimation.update(dt);
	else
		standAnimation.update(dt);

	//RESET
	if (!isTakingDamage)
		takeDamage2Animation.reset();
	if (!isFalling)
		fallAnimation.reset();
	if (!hangSlash || !isHanging)
		hangSlashAnimation.reset();
	if (!isHanging || isHangJump || isJumping) 
		hangAnimation.reset();
	if (!isRunning)
		runAnimation.reset();


	lifeTime += dt;
}
void playerRobot::setAnimations(const TextureHolder& textures) {
	runAnimation.setTexture(textures.get(Textures::Animations));
	runAnimation.setFrameSize(sf::Vector2i(80, 80));
	runAnimation.setNumFrames(16);
	runAnimation.setDuration(sf::seconds(1.2f));
	runAnimation.setRepeating(true);
	runAnimation.setTextureOffset(0, 0);
	runAnimation.setRepeatOn(2);
	runAnimation.setSign(sign);
	runAnimation.update(sf::seconds(0.01f));

	jumpAnimation.setTexture(textures.get(Textures::Animations));
	jumpAnimation.setFrameSize(sf::Vector2i(80, 80));
	jumpAnimation.setNumFrames(6);
	jumpAnimation.setDuration(sf::seconds(.6f));
	jumpAnimation.setTextureOffset(0, 80);
	jumpAnimation.setSign(sign);
	jumpAnimation.update(sf::seconds(0.01f));

	fallAnimation.setTexture(textures.get(Textures::Animations));
	fallAnimation.setFrameSize(sf::Vector2i(80, 80));
	fallAnimation.setNumFrames(6);
	fallAnimation.setDuration(sf::seconds(.8f));
	fallAnimation.setTextureOffset(480, 80);
	fallAnimation.setSign(sign);
	fallAnimation.update(sf::seconds(0.01f));

	stopSlashAnimation.setTexture(textures.get(Textures::Animations));
	stopSlashAnimation.setFrameSize(sf::Vector2i(80, 80));
	stopSlashAnimation.setNumFrames(5);
	stopSlashAnimation.setDuration(sf::seconds(.7f));
	stopSlashAnimation.setTextureOffset(0, 160);
	stopSlashAnimation.setSign(sign);
	stopSlashAnimation.update(sf::seconds(0.01f));

	slash1Animation.setTexture(textures.get(Textures::Animations));
	slash1Animation.setFrameSize(sf::Vector2i(80, 80));
	slash1Animation.setNumFrames(12);
	slash1Animation.setDuration(sf::seconds(0.3f));
	slash1Animation.setTextureOffset(0, 240);
	slash1Animation.setSign(sign);
	slash1Animation.update(sf::seconds(0.01f));

	slash2Animation.setTexture(textures.get(Textures::Animations));
	slash2Animation.setFrameSize(sf::Vector2i(80, 80));
	slash2Animation.setNumFrames(11);
	slash2Animation.setDuration(sf::seconds(0.3f));
	slash2Animation.setTextureOffset(0, 320);
	slash2Animation.setSign(sign);
	slash2Animation.update(sf::seconds(0.01f));

	slash3Animation.setTexture(textures.get(Textures::Animations));
	slash3Animation.setFrameSize(sf::Vector2i(80, 80));
	slash3Animation.setNumFrames(14);
	slash3Animation.setDuration(sf::seconds(0.4f));
	slash3Animation.setTextureOffset(0, 400);
	slash3Animation.setSign(sign);
	slash3Animation.update(sf::seconds(0.01f));

	jumpSlashAnimation.setTexture(textures.get(Textures::Animations));
	jumpSlashAnimation.setFrameSize(sf::Vector2i(80, 80));
	jumpSlashAnimation.setNumFrames(9);
	jumpSlashAnimation.setDuration(sf::seconds(0.4f));
	jumpSlashAnimation.setTextureOffset(0, 480);
	jumpSlashAnimation.setSign(sign);
	jumpSlashAnimation.update(sf::seconds(0.01f));

	dashAnimation.setTexture(textures.get(Textures::Animations));
	dashAnimation.setFrameSize(sf::Vector2i(80, 80));
	dashAnimation.setNumFrames(11);
	dashAnimation.setDuration(sf::seconds(0.5f));
	dashAnimation.setTextureOffset(0, 560);
	dashAnimation.setSign(sign);
	dashAnimation.setRepeating(true);
	dashAnimation.update(sf::seconds(0.01f));

	hangAnimation.setTexture(textures.get(Textures::Animations));
	hangAnimation.setFrameSize(sf::Vector2i(80, 80));
	hangAnimation.setNumFrames(4);
	hangAnimation.setDuration(sf::seconds(.5f));
	hangAnimation.setTextureOffset(0, 640);
	hangAnimation.setSign(sign);
	hangAnimation.update(sf::seconds(0.01f));

	hangJumpAnimation.setTexture(textures.get(Textures::Animations));
	hangJumpAnimation.setFrameSize(sf::Vector2i(80, 80));
	hangJumpAnimation.setNumFrames(3);
	hangJumpAnimation.setDuration(sf::seconds(.3f));
	hangJumpAnimation.setTextureOffset(400, 640);
	hangJumpAnimation.setSign(sign);
	hangJumpAnimation.update(sf::seconds(0.01f));

	hangSlashAnimation.setTexture(textures.get(Textures::Animations));
	hangSlashAnimation.setFrameSize(sf::Vector2i(80, 80));
	hangSlashAnimation.setNumFrames(9);
	hangSlashAnimation.setDuration(sf::seconds(0.4f));
	hangSlashAnimation.setTextureOffset(0, 720);
	hangSlashAnimation.setSign(sign);
	hangSlashAnimation.update(sf::seconds(0.01f));

	takeDamageAnimation.setTexture(textures.get(Textures::Animations));
	takeDamageAnimation.setFrameSize(sf::Vector2i(80, 80));
	takeDamageAnimation.setNumFrames(4);
	takeDamageAnimation.setDuration(sf::seconds(.5f));
	takeDamageAnimation.setTextureOffset(0, 800);
	takeDamageAnimation.setSign(sign);
	takeDamageAnimation.update(sf::seconds(0.01f));

	takeDamage2Animation.setTexture(textures.get(Textures::Animations));
	takeDamage2Animation.setFrameSize(sf::Vector2i(80, 80));
	takeDamage2Animation.setNumFrames(6);
	takeDamage2Animation.setDuration(sf::seconds(.75f));
	takeDamage2Animation.setTextureOffset(0, 880);
	takeDamage2Animation.setSign(sign);
	takeDamage2Animation.update(sf::seconds(0.01f));

	spawnAnimation.setTexture(textures.get(Textures::Animations));
	spawnAnimation.setFrameSize(sf::Vector2i(80, 80));
	spawnAnimation.setNumFrames(14);
	spawnAnimation.setDuration(sf::seconds(1.75f));
	spawnAnimation.setTextureOffset(0, 960);
	spawnAnimation.setSign(sign);
	spawnAnimation.update(sf::seconds(0.01f));

	standAnimation.setTexture(textures.get(Textures::Animations));
	standAnimation.setFrameSize(sf::Vector2i(80, 80));
	standAnimation.setNumFrames(5);
	standAnimation.setDuration(sf::seconds(2));
	standAnimation.setRepeating(true);
	standAnimation.setTextureOffset(0, 1040);
	standAnimation.setSign(sign);
	standAnimation.update(sf::seconds(0.01f));

	leaveAnimation.setTexture(textures.get(Textures::Animations));
	leaveAnimation.setFrameSize(sf::Vector2i(80, 80));
	leaveAnimation.setNumFrames(12);
	leaveAnimation.setDuration(sf::seconds(1.5f));
	leaveAnimation.setTextureOffset(0, 1120);
	leaveAnimation.setSign(sign);
	leaveAnimation.update(sf::seconds(0.01f));
}