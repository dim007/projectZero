#ifndef SCENENODE_H
#define SCENENODE_H

#include "stdafx.h"
#include <SFML\System\NonCopyable.hpp>
#include <SFML\Graphics\Drawable.hpp>
#include <SFML\Graphics\Transformable.hpp>
#include <memory>
#include <vector>
#include <cassert>
#include <SFML\System\Time.hpp>
#include <functional>
#include "Category.h"
#include "Command.h"
#include <queue>
#include <set>

class SceneNode : public sf::Transformable, public sf::Drawable, private sf::NonCopyable {
public:
	typedef std::unique_ptr<SceneNode> Ptr;
	typedef std::pair<SceneNode*, SceneNode*> Pair;

	explicit SceneNode(Category::Type category = Category::None); //if no param then category = none
	void attachChild(Ptr child);
	Ptr detachChild(const SceneNode& node);

	void update(sf::Time dt, std::queue<Command>& commands);

	sf::Transform getWorldTransform() const;
	sf::Vector2f getWorldPosition() const;

	void onCommand(const Command& command, sf::Time dt);
	virtual unsigned int getCategory() const;
	
	//Collision detection
	void checkSceneCollision(SceneNode& sceneGraph, std::set<Pair>& collisionPairs);
	void checkNodeCollision(SceneNode& node, std::set<Pair>& collisionPairs);
	void removeWrecks();
	virtual sf::FloatRect getBoundingRect() const;
	virtual bool isMarkedForRemoval() const;
	virtual bool isDestroyed() const;

private:
	//virtual allows to be overwritten in derived classes Robot, SpriteNode etc
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
	virtual void drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const;
	void drawChildren(sf::RenderTarget& target, sf::RenderStates states) const;
	void drawBoundingRect(sf::RenderTarget& target, sf::RenderStates states) const;

	virtual void updateCurrent(sf::Time dt, std::queue<Command>& commands);
	void updateChildren(sf::Time dt, std::queue<Command>& commands);
private:
	std::vector<Ptr> mChildren;
	SceneNode* mParent;
	Category::Type mCategory;
};

//gloabl? anonymous namespace?
bool collision(const SceneNode& lhs, const SceneNode& rhs);
float distance(const SceneNode& lhs, const SceneNode& rhs);
#endif