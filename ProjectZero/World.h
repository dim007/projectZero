#ifndef WORLD_H
#define WORLD_H
//^ prevents compiler from generating class more than once
#include "stdafx.h"
#include "ResourceHolder.h"
#include "SpriteNode.h"
#include "Platform.h"
#include "SFML\Graphics\RenderWindow.hpp"
#include <memory>
#include <array>
#include <queue>
#include "Category.h" 
#include "playerRobot.h"
#include "EnemyRobot.h"

namespace sf { //Forward declaration
	class RenderWindow;
}
class World : private sf::NonCopyable {
private:
	enum Layer { //layer naming
		BackgroundLayer, 
		PlatformLayer,
		ForegroundLayer,
		LayerCount
	};
	struct SpawnPoint {
		SpawnPoint(Robot::RobotType type, float x, float y)
			: type(type),
			x(x),
			y(y)
		{
		}

		Robot::RobotType type;
		float x;
		float y;
	};
public:
	typedef ResourceHolder<sf::Texture, Textures::ID> TextureHolder; //shortcut for typing
	typedef ResourceHolder<sf::Font, Fonts::ID> FontHolder;

	explicit World(sf::RenderWindow& window, FontHolder& fonts);
	void update(sf::Time dt);
	void draw();
	std::queue<Command>& getCommandQueue();

	bool hasAlivePlayer() const;
	bool hasPlayerReachedEnd() const;
	void setPlayerLives(int lives);

private:
	void loadTextures(); //adds textures to sprites
	void buildScene(); //builds the scene graph

	void adaptPlayerPosition();
	void adaptViewPosition();
	void adaptPlayerVelocity();
	void handleCollisions();

	void spawnEnemies();
	void addEnemies();
	void addEnemy(Robot::RobotType type, float relX, float relY);
	void destroyEntitiesOutsideView();
	void guideMissiles();
	sf::FloatRect getBattlefieldBounds() const;
	sf::FloatRect getViewBounds() const;

	sf::Time startTime;

private:
	//Variables
	sf::RenderWindow& mWindow;
	FontHolder& mFonts;
	sf::View mWorldView;
	TextureHolder mTextures;
	SceneNode mSceneGraph;
	std::array<SceneNode*, LayerCount>  mSceneLayers;

	sf::FloatRect mWorldBounds;
	sf::Vector2f mSpawnPosition;
	playerRobot* mPlayerRobot;
	sf::Vector2f mPlayerPosition;

	std::vector<SpawnPoint> mEnemySpawnPoints;
	std::vector<Robot*> mActiveEnemies;
	std::queue<Command> mCommandQ;
	float worldGravity;

};
#endif