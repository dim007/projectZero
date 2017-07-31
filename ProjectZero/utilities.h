#ifndef UTILITIES
#define UTILITIES
#include "stdafx.h"
#include "SFML\Graphics.hpp"
#include <cmath>
#include "Animation.h"
#include <random>
#include <cassert>

//inline needed to use declared methods in header, else need cpp file for body
//without inline, multiple defined methods will exist causing error (ifndef does not help)


inline void centerOrigin(sf::Sprite& sprite) {
	sf::FloatRect bounds = sprite.getLocalBounds();
	//width/2 added to left, left being position in world
	sprite.setOrigin(std::floor(bounds.left + bounds.width / 2.f), std::floor(bounds.top + bounds.height / 2.f)); //x,y
}
inline void centerOrigin(sf::Text& text) {
	sf::FloatRect bounds = text.getLocalBounds();
	text.setOrigin(std::floor(bounds.left + bounds.width / 2.f), std::floor(bounds.top + bounds.height / 2.f));
}
inline void centerOrigin(Animation& animation) {
	sf::FloatRect bounds = animation.getLocalBounds();
	animation.setOrigin(std::floor(bounds.left + bounds.width / 2.f), std::floor(bounds.top + bounds.height / 2.f));
}
inline float toDegree(float radian)
{
	return 180.f / 3.141592653589793238462643383f * radian;
}

inline float toRadian(float degree)
{
	return 3.141592653589793238462643383f / 180.f * degree;
}
inline int randomInt(int exclusiveMax)
{
	auto seed = static_cast<unsigned long>(std::time(nullptr));
	auto engine = std::default_random_engine(seed);
	std::uniform_int_distribution<> distr(0, exclusiveMax - 1);
	return distr(engine);
}
inline float length(sf::Vector2f vector)
{
	return std::sqrt(vector.x * vector.x + vector.y * vector.y);
}
inline sf::Vector2f unitVector(sf::Vector2f vector)
{
	assert(vector != sf::Vector2f(0.f, 0.f));
	return vector / length(vector);
}
#endif