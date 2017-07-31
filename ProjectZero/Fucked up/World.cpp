#include "stdafx.h"
#include <iostream>
#include "World.h"
#include <memory>
#include "Pickup.h"
using namespace std;

World::World(sf::RenderWindow& window, FontHolder& fonts)
	: mWindow(window),
	//zooms into the world below
	//mWorldView(sf::FloatRect(0,0,450,250)), //sf::View view(origin.x, origin.y, size.width, size.height);
	mWorldView(window.getDefaultView()),
	mTextures(),
	mSceneGraph(),
	mSceneLayers(),
	mWorldBounds(0.f, 0.f, 7677.f, 1000), //world(left x pos, top y pos, max width, max height)
	mSpawnPosition(mWorldView.getSize().x / 2.f, mWorldView.getSize().y / 2.f),
	mPlayerRobot(nullptr),
	mCommandQ(),
	mFonts(fonts),
	mEnemySpawnPoints(),
	mActiveEnemies(),
	worldGravity(300.f)
{
	loadTextures();
	buildScene();

	//prepare View
	mWorldView.setCenter(mSpawnPosition);
	adaptViewPosition();

}
void World::loadTextures() {
	mTextures.load(Textures::Zero, "zero_sprites/proto_zero.png");
	mTextures.load(Textures::Background, "textures/x_background.png");
	mTextures.load(Textures::Highway, "textures/platform1.png");
	mTextures.load(Textures::X, "zero_sprites/stand1.png");
	mTextures.load(Textures::Drone, "textures/Drone.png");
	mTextures.load(Textures::Slash1, "zero_sprites/slash1.png");
	mTextures.load(Textures::Slash2, "zero_sprites/slash2.png");
	mTextures.load(Textures::Slash3, "zero_sprites/slash3.png");

	mTextures.load(Textures::Bullet, "textures/Bullet.png");
	mTextures.load(Textures::Missile, "textures/Missile.png");
	mTextures.load(Textures::HealthRefill, "textures/HealthRefill.png");
	mTextures.load(Textures::MissileRefill, "textures/MissileRefill.png");
	mTextures.load(Textures::FireRate, "textures/FireRate.png");
	mTextures.load(Textures::FireSpread, "textures/FireSpread.png");
}
void World::buildScene() {
	//build graph
	for (std::size_t i = 0; i < LayerCount; i++) { //adds layer nodes under root
		//Setting the ActionLayer below, actionLayer is for Projectiles command found in Robot.cpp constructor
		//Only ActionLayer can use the projectile commands, and its children
		//Having actionLayer prevents duplicate bullets, nothing else may use actionLayer
		Category::Type category = (i == PlatformLayer) ? Category::ActionLayer : Category::None;
		SceneNode::Ptr layer(new SceneNode(category));
		mSceneLayers[i] = layer.get();
		mSceneGraph.attachChild(std::move(layer));
	}
	//instantiate backgound textue as repeated/tiled
	sf::Texture& backTexture = mTextures.get(Textures::Background);
	sf::IntRect textureRect(mWorldBounds); //rect size of world
	backTexture.setRepeated(false); //tiles a image if true

	//Add background sprite to scene
	std::unique_ptr<SpriteNode> backgroundSprite(new SpriteNode(backTexture, textureRect));
	auto size = backTexture.getSize();
	backgroundSprite->setScale(mWorldBounds.width/size.x, mWorldBounds.height/size.y); //THIS sclaes image to fit perfectly to window
	backgroundSprite->setPosition(mWorldBounds.left, mWorldBounds.top); //moves topleft corner to top left of world
	mSceneLayers[BackgroundLayer]->attachChild(std::move(backgroundSprite)); //adds under node Background

	//Load platform sprite to scene
	std::unique_ptr<Platform> platformSprite(new Platform(Platform::Highway, mTextures, sf::IntRect(0, 0, 1000, 200)));//104)));
	platformSprite->setPosition(mWorldBounds.left, mWorldBounds.top + 500.f);
	mSceneLayers[PlatformLayer]->attachChild(std::move(platformSprite));


	//Add player character
	std::unique_ptr<Robot> player(new Robot(Robot::Zero, mTextures, mFonts));
	mPlayerRobot = player.get();
	mPlayerRobot->setPosition(mSpawnPosition);
	mPlayerRobot->setGravity(worldGravity);
	mSceneLayers[PlatformLayer]->attachChild(std::move(player));
	/*NOTE: mPlayerRobot extends entity which extends SceneNode, mPlayerRobot can be
	treated as a node in graph too, with its own children making their position relative to the parent
	in this case mPlayerRobot is the parent*/

	//enemy spawn
	addEnemies();
}
void World::draw() {
	mWindow.setView(mWorldView);
	mWindow.draw(mSceneGraph); //will iterate through all nodes calling draw on each
}
void World::update(sf::Time dt) { 
	//mWorldView.move(mScrollSpeed * dt.asSeconds(), 0.f); //x,y allows iew to autoscroll right
	
	mPlayerRobot->setVelocity(0.f, 0.f);

	//Guide missile, destroy offscreen
	destroyEntitiesOutsideView(); //Destroy before guiding missiles
	guideMissiles();

	//**VELOCITY/COMMAND modification section, must be before mSceneGraph.update()
	//Forward commands to scenegraph
	while (!mCommandQ.empty()) {
		mSceneGraph.onCommand(mCommandQ.front(), dt);
		mCommandQ.pop();
	}
	//adaptPlayerVelocity(); //diagnonal correction, not needed for sidescroller
	//**VELOCITY/COMMAND section ended

	mSceneGraph.removeWrecks(); //NOTE:: Remove wrecks MUST be after Commands. Pickup spawn command must happen before removing wrecks
	spawnEnemies();

	//NOTE: must update Scene AFTER changing velocities of entities
	//Any velocity changes after the update wont affect the Scene
	mSceneGraph.update(dt, mCommandQ);

	//Collision detection
	handleCollisions();
	adaptPlayerPosition(); //prevent going out of world
	adaptViewPosition();  //prevent camera from going outside of world
}
std::queue<Command>& World::getCommandQueue() {
	return mCommandQ;
}


void World::spawnEnemies() {
	//spawns enemies entering the battlefield bounds
	while (!mEnemySpawnPoints.empty() && (mEnemySpawnPoints.back().y > getBattlefieldBounds().top
		|| mEnemySpawnPoints.back().x < getBattlefieldBounds().width + getBattlefieldBounds().left)) {

		SpawnPoint spawn = mEnemySpawnPoints.back(); //get the closest enemy to us

		std::unique_ptr<Robot> enemy(new Robot(spawn.type, mTextures, mFonts));
		enemy->setPosition(spawn.x, spawn.y);
		enemy->setGravity(worldGravity);
		//enemy->setRotation(180.f); //rotates sprite by x degrees

		mSceneLayers[PlatformLayer]->attachChild(std::move(enemy));

		mEnemySpawnPoints.pop_back(); //erase the back, sorted from greatest y value to least
	}
}
void World::addEnemies() {
	//add enemies to vector list
	addEnemy(Robot::X, 300.f, 500.f);
	addEnemy(Robot::Drone, 500.f, 100.f);

	//sort enemies based on position
	//std::sort(mEnenmySpawnPoints.begin(), mEnenmySpawnPoints.end(), sortFunction); //sortFunction is replaced with lambda
	//Can also use lambda function
	std::sort(mEnemySpawnPoints.begin(), mEnemySpawnPoints.end(), [] (SpawnPoint lhs, SpawnPoint rhs) { return lhs.x > rhs.x; });
}

void World::addEnemy(Robot::RobotType type, float relX, float relY) {
	//player spawn is the origin
	SpawnPoint spawn(type, mSpawnPosition.x + relX, mSpawnPosition.y - relY);
	mEnemySpawnPoints.push_back(spawn);
}
bool World::hasAlivePlayer() const {
	return !mPlayerRobot->isMarkedForRemoval();
}
bool World::hasPlayerReachedEnd() const {
	return !mWorldBounds.contains(mPlayerRobot->getPosition());
}
void World::adaptViewPosition() {
	//keep view inside world
	//original line below, followed player
	//mWorldView.setCenter(mPlayerRobot->getPosition().x, mWorldView.getCenter().y); //center after velocities updated

	sf::Vector2f viewSize = mWorldView.getSize();

	sf::Vector2f position = mPlayerRobot->getPosition(); //use player position as new center, then modify below
	position.x = std::max(position.x, mWorldBounds.left + (viewSize.x / 2.f)); //keeps view from going too left
	position.x = std::min(position.x, mWorldBounds.width - (viewSize.x / 2.f)); // keeps from going too right
	position.y = std::max(position.y, mWorldBounds.top + (viewSize.y / 2.f));
	position.y = std::min(position.y, mWorldBounds.height - (viewSize.y / 2.f));
	mWorldView.setCenter(position.x, position.y);
}
void World::adaptPlayerPosition() {
	//keep player inside the screen bounds, with a boarder around bounds
	//sf::FloatRect viewBounds = getViewBounds();
	sf::FloatRect Bounds = mWorldBounds;
	
	sf::Vector2f position = mPlayerRobot->getPosition();
	//set player death if touches the bottom of world
	if (position.y >= Bounds.height) {
		mPlayerRobot->destroy();
	}

	//player boarders
	position.x = std::max(position.x, Bounds.left); //keeps from going left
	position.x = std::min(position.x, Bounds.left + Bounds.width); //keeps from going right
	position.y = std::max(position.y, Bounds.top); //keeps from going to high
	position.y = std::min(position.y, Bounds.top + Bounds.height); //keeps from going to low
	mPlayerRobot->setPosition(position);
}

void World::adaptPlayerVelocity() {
	
	sf::Vector2f velocity = mPlayerRobot->getVelocity();
	//reduce velocity if moving diagonal, to maintain same velocity
	if (velocity.x != 0.f && velocity.y != 0.f)
		mPlayerRobot->setVelocity(velocity / std::sqrt(2.f));

}

bool matchesCategories(SceneNode::Pair& colliders, Category::Type type1, Category::Type type2) {
	
	unsigned int category1 = colliders.first->getCategory();
	unsigned int category2 = colliders.second->getCategory();

	//check if first Pair have category1 == type1, cat2 == type2
	if (type1 & category1 && type2 & category2)
		return true;
	else if (type1 & category2 && type2 & category1) {
		std::swap(colliders.first, colliders.second);
		return true;
	}
	else
		return false;
}

void World::handleCollisions() {
	std::set<SceneNode::Pair> collisionPairs;
	mSceneGraph.checkSceneCollision(mSceneGraph, collisionPairs);

	for (SceneNode::Pair pair : collisionPairs) {
		if (matchesCategories(pair, Category::PlayerRobot, Category::EnemyRobot)) {
			auto& player = static_cast<Robot&>(*pair.first);
			auto& enemy = static_cast<Robot&>(*pair.second);

			//Collision: Player damage
			player.damage(10);
			enemy.destroy();
		}
		else if (matchesCategories(pair, Category::PlayerRobot, Category::Pickup)) {
			auto& player = static_cast<Robot&>(*pair.first);
			auto& pickup = static_cast<Pickup&>(*pair.second);

			//Apply effect to player
			pickup.apply(player);
			pickup.destroy();
		}
		else if (matchesCategories(pair, Category::EnemyRobot, Category::AlliedProjectile)
			|| matchesCategories(pair, Category::PlayerRobot, Category::EnemyProjectile)) {

			auto& robot = static_cast<Robot&>(*pair.first);
			auto& projectile = static_cast<Projectile&>(*pair.second);

			//Apply projectile damage
			robot.damage(projectile.getDamage());
			projectile.destroy();
		}
		if (matchesCategories(pair, Category::Robot, Category::Platform)) {
			auto& robot = static_cast<Robot&>(*pair.first);
			auto& platform = static_cast<Platform&>(*pair.second);
			sf::Vector2f position = robot.getWorldPosition();
			sf::FloatRect platformRect = platform.getBoundingRect();
			sf::FloatRect robotRect = robot.getBoundingRect();
			//top collision
			if (platformRect.left - robotRect.width / 2.f < position.x
				&& position.x < platformRect.left + platformRect.width + robotRect.width / 2.f
				&& position.y < platformRect.top) 
			{
				position.y = std::min(position.y, platform.getWorldPosition().y - robot.getBoundingRect().height / 2.f);
				robot.isGrounded = true;
			}
			//side collision
			else if (platformRect.top <= position.y && position.y <= platformRect.top + platformRect.height) {
				robot.sliding = true;
				if (platformRect.left + platformRect.width < position.x)
					position.x = std::max(position.x, platformRect.left + platformRect.width + robotRect.width / 2.f);
				else if (position.x < platformRect.left)
					position.x = std::min(position.x, platformRect.left - robotRect.width / 2.f);
			}
			robot.setPosition(position);
		}
	}
}

void World::destroyEntitiesOutsideView() {

	Command command;
	command.category = Category::Projectile | Category::EnemyRobot;
	//Lamda function:
	//[capture/return list, a = by value, &a by referance] (Params) {body}
	//[this] applies to all Entities under Projectile | EnemyRobot, since they will be calling this action
	command.action = derivedAction<Entity>([this](Entity& enemy, sf::Time)
	{
		if (!getBattlefieldBounds().intersects(enemy.getBoundingRect()))
			enemy.destroy();
	});

	mCommandQ.push(command);
}
void World::guideMissiles() {
	//Set command that stores active enemies to vector
	Command enemyCollector;
	enemyCollector.category = Category::EnemyRobot;
	enemyCollector.action = derivedAction<Robot>([this](Robot& enemy, sf::Time)
	{
		if (!enemy.isDestroyed())
			mActiveEnemies.push_back(&enemy);
	});

	//Set command that guides missiles to closest enemy
	Command missileGuider;
	missileGuider.category = Category::AlliedProjectile;
	missileGuider.action = derivedAction<Projectile>([this](Projectile& missile, sf::Time) 
	{
		if (!missile.isGuided())
			return;

		float minDistance = std::numeric_limits<float>::max(); //set distance to max
		Robot* closestEnemy = nullptr;
		//Find enemy
		for (Robot* enemy : mActiveEnemies) {
			float enemyDistance = distance(missile, *enemy); //distance() in utilities.h
			if (enemyDistance < minDistance) {
				closestEnemy = enemy;
				minDistance = enemyDistance;
			}
		}
		if (closestEnemy)
			missile.guideTowards(closestEnemy->getWorldPosition());
	});
	//push commands
	mCommandQ.push(enemyCollector);
	mCommandQ.push(missileGuider);
	mActiveEnemies.clear();
}
//set battlefield bounds larger than real bounds for seamless spawning
sf::FloatRect World::getBattlefieldBounds() const {
	sf::FloatRect bounds = getViewBounds();
	bounds.height += 100.f;
	bounds.top -= 100.f;
	bounds.width += 200.f; //compensate for right and left
	bounds.left -= 100.f;
	return bounds;
}
sf::FloatRect World::getViewBounds() const {
	return sf::FloatRect(mWorldView.getCenter() - mWorldView.getSize() / 2.f, mWorldView.getSize());
}