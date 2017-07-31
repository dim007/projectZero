#ifndef COMMAND
#define COMMAND
#include "stdafx.h"
#include <cassert>
#include <functional>
#include <SFML\System\Time.hpp>
#include <SFML\System.hpp>
#include <SFML\Graphics.hpp>


class SceneNode;

struct Command {
	Command() {
	}
	//void is return type, then parameters in parens
	std::function<void(SceneNode&, sf::Time)> action;
	unsigned int category;
};


template <typename GameObject, typename Function>
std::function<void(SceneNode&, sf::Time)> derivedAction(Function fn) {
	return [=](SceneNode& node, sf::Time dt) {
		assert(dynamic_cast<GameObject*>(&node) != nullptr);
		//Down cast to SceneNode from extended class of SceneNode
		fn(static_cast<GameObject&>(node), dt);
	};
}

#endif