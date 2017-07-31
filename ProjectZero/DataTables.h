#pragma once
#include "stdafx.h"
#include "TextureID.h"
#include <vector>
#include "SFML\System.hpp"
#include <functional>
#include "Attack.h"
#include "Projectile.h"
using namespace std;

class Robot; //robot.h is include in cpp file not in header, rn its a fake class
struct Direction {
	Direction(float angle, float distance)
		: angle(angle), distance(distance) 
	{
	}
	float angle;
	float distance;
};
struct RobotData {
	int hitpoints;
	float speed;
	bool flight;
	Textures::ID texture;
	sf::Time fireInterval;
	int fireAmount;
	std::vector<Direction> directions;
	

	//animations
	bool hasAttackAnim, hasShootAnim;
	int runNumFrames, attackNumFrames, shootNumFrames;
	sf::Time runDuration, attackDuration, shootDuration;

	Attack::Type attack;
	Projectile::Type projectile;

	sf::IntRect hitBox;
	vector<sf::IntRect> runFramesVector, attackFramesVector, shootFramesVector;
	float attackDistanceHeight;
	float attackDistanceWidth;
	sf::Time movementAllowance, movementCooldown, attackCooldown;
	int attackFrame;
};
struct ProjectileData {
	int damage;
	float speed;
	Textures::ID texture;
};
struct PickupData {
	std::function<void(Robot&)> action;
	Textures::ID texture;
};

//global functions, no class
std::vector<RobotData> initializeRobotData();
std::vector<ProjectileData> initializeProjectileData();
std::vector<PickupData> initializePickupData();
