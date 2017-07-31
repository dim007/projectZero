#pragma once
#include "stdafx.h"
#include "Robot.h"
#include "Command.h"
#include <vector>

using namespace std;

class EnemyRobot : public Robot {
public:
	EnemyRobot(const RobotType type, const TextureHolder& textures, const FontHolder& fonts, sf::Vector2f& playerPosition);
	unsigned int getCategory() const;
	virtual sf::FloatRect getBoundingRect() const;

	bool isAttacking, isRunning, isShooting;
private:
	virtual void drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const;
	void updateCurrent(sf::Time dt, std::queue<Command>& commands);

	void handleMovement(sf::Time dt);
	void checkProjectileLaunch(sf::Time dt, std::queue<Command>& commands);
	void checkAttack(sf::Time dt, std::queue<Command>& commands);
	void createAttack(SceneNode& node, Attack::Type type, float xOffset, float yOffset, const TextureHolder& textures);
	void setAnimations(const TextureHolder& textures);
	void setupCommands(const TextureHolder& textures);
	void handleAnimationUpdate(sf::Time dt);

	Textures::ID mTextureID;
	RobotType mType;
	sf::Sprite mHitBox;
	sf::Vector2f* mPlayerPosition;
	Command attackCommand, shootCommand;
	sf::Time movementAllowance;
	sf::Time movementCooldown;
	sf::Time attackCooldown;
	sf::Time attackAllowance;

	Animation runAnimation, attackAnimation, shootAnimation;
};