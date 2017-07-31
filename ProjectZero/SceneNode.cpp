#include "stdafx.h"
#include "SceneNode.h"
#include <cmath>
#include <algorithm>
#include "utilities.h"
#include <iostream>

using namespace std;
SceneNode::SceneNode(Category::Type category)
	: mChildren(),
	mParent(nullptr),
	mCategory(category)
{
}
void SceneNode::attachChild(Ptr child) {
	child->mParent = this;
	mChildren.push_back(std::move(child));
}
SceneNode::Ptr SceneNode::detachChild(const SceneNode& node) {
	//std::find_if iterates to find given node
	//[&] finds variables by reference address
	auto found = std::find_if(mChildren.begin(), mChildren.end(), [&](Ptr& p) {return p.get() == &node; });
	assert(found != mChildren.end());

	Ptr result = std::move(*found);
	result->mParent = nullptr;
	mChildren.erase(found);
	return result;
}
void SceneNode::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	//Apply tranform of current node
	states.transform *= getTransform(); //mult parent absolute transform with relative childposition, to get absolute position
	
										
	//Draw children with changed transform
	drawCurrent(target, states);
	drawChildren(target, states);

	//***Draw bounding rectangles. Debuging
	drawBoundingRect(target, states);
}
void SceneNode::drawChildren(sf::RenderTarget& target, sf::RenderStates states) const {
	for (const Ptr& child : mChildren)
		child->draw(target, states);
}
//virtual allows to be overwritten
void SceneNode::drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const {
	//Does Nothing?
}
void SceneNode::drawBoundingRect(sf::RenderTarget& target, sf::RenderStates states) const {
	sf::FloatRect rect = getBoundingRect();

	sf::RectangleShape shape;
	shape.setPosition(sf::Vector2f(rect.left, rect.top));
	shape.setSize(sf::Vector2f(rect.width, rect.height));
	shape.setFillColor(sf::Color::Transparent);
	shape.setOutlineColor(sf::Color::Green);
	shape.setOutlineThickness(1.f);
	target.draw(shape);
}
void SceneNode::update(sf::Time dt, std::queue<Command>& commands) {
	updateCurrent(dt, commands);
	updateChildren(dt, commands);
}
void SceneNode::updateCurrent(sf::Time dt, std::queue<Command>&) {
	//Do nothing, wait for override
}
void SceneNode::updateChildren(sf::Time dt, std::queue<Command>& commands) {
	for (Ptr& child : mChildren)
		child->update(dt, commands);
}
sf::Transform SceneNode::getWorldTransform() const {
	sf::Transform transform = sf::Transform::Identity;
	for (const SceneNode* node = this; node != nullptr; node = node->mParent) {
		transform = node->getTransform() * transform; //careful when putting *, might dereference accidentally
	}
	return transform;
}
sf::Vector2f SceneNode::getWorldPosition() const {
	return getWorldTransform() * sf::Vector2f();
}

void SceneNode::onCommand(const Command& command, sf::Time dt) {
	if (command.category & getCategory()) //if given command is for/ matches current scenenode's category
		command.action(*this, dt); //then this scenenode takes on the parameter command as its "action" definition
	for (Ptr& child : mChildren)
		child->onCommand(command, dt); //repeat for all children
}

unsigned int SceneNode::getCategory() const {
	return mCategory;
}
void SceneNode::checkSceneCollision(SceneNode& sceneGraph, std::set<Pair>& collisionPairs) {
	checkNodeCollision(sceneGraph, collisionPairs);
	for (Ptr& child : sceneGraph.mChildren) //I magically found a bug here!!!!! Jk but makes sense
		checkSceneCollision(*child, collisionPairs);
}
void SceneNode::checkNodeCollision(SceneNode& node, std::set<Pair>& collisionPairs) {
	if (this != &node && collision(*this, node) && !isDestroyed() && !node.isDestroyed())
		collisionPairs.insert(std::minmax(this, &node));
	for (Ptr& child : mChildren)
		child->checkNodeCollision(node, collisionPairs);
}
void SceneNode::removeWrecks() {
	//remove children
	//wreckfield makes a list if funtion isMarked is true
	//3rd param calls the function isMarked, for each child
	auto wreckFieldBegin = std::remove_if(mChildren.begin(), mChildren.end(), std::mem_fn(&SceneNode::isMarkedForRemoval));
	//then removes all on that list
	mChildren.erase(wreckFieldBegin, mChildren.end());

	//calls removeWrecks for each child, for ther children
	std::for_each(mChildren.begin(), mChildren.end(), std::mem_fn(&SceneNode::removeWrecks));
}
sf::FloatRect SceneNode::getBoundingRect() const {
	return sf::FloatRect(); //overwritten in sub classes
}
bool SceneNode::isMarkedForRemoval() const {
	return isDestroyed();
}
bool SceneNode::isDestroyed() const {
	//overwritten in entity
	return false; //sceneNodes never need to be removed, entities/robots and other subclasses do
}
bool collision(const SceneNode& lhs, const SceneNode& rhs) {
	return lhs.getBoundingRect().intersects(rhs.getBoundingRect());
}
float distance(const SceneNode& lhs, const SceneNode& rhs) {
	//length found in utilites.h
	return length(lhs.getWorldPosition() - rhs.getWorldPosition());
}