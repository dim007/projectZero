#include "stdafx.h"
#include "DataTables.h"
#include "TextureID.h"
#include "Robot.h"

std::vector<RobotData> initializeRobotData() { //doesnt need DataTables:: bc it is not a class
	std::vector<RobotData> data(Robot::TypeCount); //Typecount is the last type, acts as a counter

	data[Robot::Zero].hitpoints = 100;
	data[Robot::Zero].speed = 200.f;
	data[Robot::Zero].texture = Textures::Animations;
	data[Robot::Zero].fireInterval = sf::seconds(1);
	data[Robot::Zero].fireAmount = 4;
	data[Robot::Zero].flight = false;

	vector<sf::IntRect> frames(10);
	data[Robot::Drone].hitpoints = 10;
	data[Robot::Drone].speed = 300.f;
	data[Robot::Drone].texture = Textures::Enemies;
	data[Robot::Drone].fireInterval = sf::seconds(2);
	data[Robot::Drone].fireAmount = 3;
	data[Robot::Drone].flight = true;
	data[Robot::Drone].runNumFrames = 10;
	data[Robot::Drone].runDuration = sf::seconds(1.f);
	data[Robot::Drone].hasAttackAnim = false;
	data[Robot::Drone].hasShootAnim = false;
	data[Robot::Drone].projectile = Projectile::EnemyBullet;
	frames[0] = sf::IntRect(496, 0, 29, 34);
	frames[1] = sf::IntRect(526, 0, 29, 34);
	frames[2] = sf::IntRect(556, 0, 29, 34);
	frames[3] = sf::IntRect(586, 0, 29, 34);
	frames[4] = sf::IntRect(616, 0, 29, 34);
	frames[5] = sf::IntRect(646, 0, 29, 34);
	frames[6] = sf::IntRect(676, 0, 29, 34);
	frames[7] = sf::IntRect(706, 0, 29, 34);
	frames[8] = sf::IntRect(736, 0, 29, 34);
	frames[9] = sf::IntRect(766, 0, 29, 34);
	data[Robot::Drone].hitBox = sf::IntRect(0,0,29, 34);
	data[Robot::Drone].runFramesVector = frames;
	data[Robot::Drone].attackDistanceHeight = 100;
	data[Robot::Drone].attackDistanceWidth = 100;
	data[Robot::Drone].movementAllowance = sf::seconds(2.f);
	data[Robot::Drone].movementCooldown = sf::seconds(0.5);

	

	data[Robot::slashSuit].hitpoints = 300;
	data[Robot::slashSuit].speed = 150.f;
	data[Robot::slashSuit].flight = false;
	data[Robot::slashSuit].texture = Textures::Enemies;
	data[Robot::slashSuit].hasShootAnim = false;
	data[Robot::slashSuit].hasAttackAnim = true;
	data[Robot::slashSuit].runNumFrames = 3;
	data[Robot::slashSuit].attackNumFrames = 6; //have to put +1 
	data[Robot::slashSuit].runDuration = sf::seconds(0.5f);
	data[Robot::slashSuit].attackDuration = sf::seconds(0.8f);
	data[Robot::slashSuit].attack = Attack::slashSuit;
	data[Robot::slashSuit].hitBox = sf::IntRect(0, 36, 59, 72);
	frames[0] = sf::IntRect(0, 36, 59, 72);
	frames[1] = sf::IntRect(63, 36, 59, 72);
	frames[2] = sf::IntRect(125, 36, 59, 72);
	data[Robot::slashSuit].runFramesVector = frames;
	frames[0] = sf::IntRect(188, 24, 90, 84);
	frames[1] = sf::IntRect(288, 17, 90, 91);
	frames[2] = sf::IntRect(386, 0, 92, 108);
	frames[3] = sf::IntRect(122, 108, 92, 116);
	frames[4] = sf::IntRect(309, 123, 114, 108);
	data[Robot::slashSuit].attackFramesVector = frames;
	data[Robot::slashSuit].attackDistanceWidth = 70;
	data[Robot::slashSuit].movementAllowance = sf::seconds(2.f);
	data[Robot::slashSuit].movementCooldown = sf::seconds(0.5f);
	data[Robot::slashSuit].attackCooldown = sf::seconds(2.f);
	data[Robot::slashSuit].attackFrame = 5;

	return data;
}
std::vector<ProjectileData> initializeProjectileData() {
	std::vector<ProjectileData> data(Projectile::TypeCount);

	data[Projectile::AlliedBullet].damage = 10;
	data[Projectile::AlliedBullet].speed = 900.f;
	data[Projectile::AlliedBullet].texture = Textures::Bullet;

	data[Projectile::EnemyBullet].damage = 10;
	data[Projectile::EnemyBullet].speed = 900.f;
	data[Projectile::EnemyBullet].texture = Textures::Bullet;

	data[Projectile::Missile].damage = 200;
	data[Projectile::Missile].speed = 500.f;
	data[Projectile::Missile].texture = Textures::Missile;

	return data;
}
std::vector<PickupData> initializePickupData() {
	std::vector<PickupData> data(Pickup::TypeCount);

	data[Pickup::HealthRefill].texture = Textures::HealthRefill;
	data[Pickup::HealthRefill].action = [](Robot& a) {a.repair(25); };

	//bind returns a funtional object, method collectMissiles, with the inputs _1, 3
	//table now holds function collectMissiles with int 3 as input
	// _1 represents a variable, the action is called data[].action(x), with x as the variable bc of _1
	data[Pickup::MissileRefill].texture = Textures::MissileRefill;
	data[Pickup::MissileRefill].action = std::bind(&Robot::collectMissiles, std::placeholders::_1, 3);

	//data[Pickup::FireSpread].texture = Textures::FireSpread;
	//data[Pickup::FireSpread].action = std::bind(&Robot::increaseSpread, std::placeholders::_1);

	//changed 9/10 4am
	//data[Pickup::FireRate].texture = Textures::FireRate;
	//data[Pickup::FireRate].action = std::bind(&Robot::increaseFireRate, std::placeholders::_1);

	//Major bug found at 8/18 12:05, I used Pickup::MissileRefill for the above so other pickup textures did not exist, game crashed when it
	//was trying to be created.
	return data;
}