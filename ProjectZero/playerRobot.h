#pragma once
#include "stdafx.h"
#include "Robot.h"
#include "DataTables.h"

class playerRobot : public Robot {
public:

	playerRobot(RobotType type, const TextureHolder& textures, const FontHolder& fonts);
	unsigned int getCategory() const;
	virtual sf::FloatRect getBoundingRect() const;


	void checkJump(sf::Time dt);
	void checkDash(sf::Time dt);
	bool isMarkedForRemoval() const;

	bool isJumping, isHangJump, isTakingDamage;
	bool isDashing, canDash, airDashing;
	bool slashing1, slashing2, slashing3, stopSlash, jumpSlash, hangSlash;
	sf::Time jumpTime, jumpCooldown;
	int attackCount;
	sf::Time dashTime;

	float damageDirection;

private:
	virtual void drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const;
	void updateCurrent(sf::Time dt, std::queue<Command>& commands);

	void checkTakeDamage(sf::Time dt);
	void checkAttack(sf::Time dt, std::queue<Command>& commands);
	void createAttack(SceneNode& node, Attack::Type type, float xOffset, float yOffset, const TextureHolder& textures);
	void setAnimations(const TextureHolder& textures);
	void setupCommands(const TextureHolder& textures);
	void handleAnimationUpdate(sf::Time dt);

	Textures::ID mTextureID;
	RobotType mType;
	sf::Sprite mHitBox;
	sf::Time lifeTime, deathTime;

	bool isSpawning, isRunning, isFalling;
	Animation runAnimation, jumpAnimation, fallAnimation, stopSlashAnimation, slash1Animation, slash2Animation, slash3Animation, jumpSlashAnimation,
		dashAnimation, hangAnimation, hangJumpAnimation, hangSlashAnimation, takeDamageAnimation, takeDamage2Animation, spawnAnimation, standAnimation, leaveAnimation,
		deathAnimation;

	Command slash1Command, slash2Command, slash3Command, jumpSlashCommand, hangSlashCommand;
	sf::Time attackCountdown, takeDamageCountDown;

};