#pragma once
#include "stdafx.h"
#include <SFML\Graphics.hpp>
#include <SFML\System\Time.hpp>
#include <vector>
using namespace std;
class Animation : public sf::Drawable, public sf::Transformable {
public:
	Animation();
	explicit Animation(const sf::Texture& texture);

	void 					setTexture(const sf::Texture& texture);
	const sf::Texture* 		getTexture() const;

	void 					setFrameSize(sf::Vector2i mFrameSize);
	sf::Vector2i		 	getFrameSize() const;

	void 					setNumFrames(std::size_t numFrames);
	std::size_t 			getNumFrames() const;

	void 					setDuration(sf::Time duration);
	sf::Time 				getDuration() const;

	void 					setRepeating(bool flag);
	bool 					isRepeating() const;

	void					setTextureOffset(int x, int y);
	sf::Vector2i			textureOffset;

	void					setRepeatOn(int frame);
	int						mRepeatFrame;

	void	reset(); //resets loop
	void	setSign(float& sign);
	void setVectorFrames(vector<sf::IntRect> frames);

	void 					restart();
	bool 					isFinished() const;

	sf::FloatRect 			getLocalBounds() const;
	sf::FloatRect 			getGlobalBounds() const;

	void 					update(sf::Time dt);

	std::size_t mCurrentFrame;

private:
	void 					draw(sf::RenderTarget& target, sf::RenderStates states) const;
	sf::IntRect flip(sf::IntRect textureRect);
private:
	sf::Sprite mSprite;
	float* mSign; //will point to the sign of the Robot owner
	sf::Vector2i mFrameSize;
	std::size_t mNumFrames;
	sf::Time mDuration;
	sf::Time mElapsedTime;
	bool mRepeat;
	vector<sf::IntRect> vecFrames;
	bool hasVectorFrames;
};