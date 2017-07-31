#include "stdafx.h"
#include "Animation.h"
#include "utilities.h"
#include <iostream>
using namespace std;

Animation::Animation() 
	: mSprite()
	, mFrameSize()
	, mNumFrames(0)
	, mCurrentFrame(0)
	, mDuration(sf::Time::Zero)
	, mElapsedTime(sf::Time::Zero)
	, mRepeat(false),
	textureOffset(0,0),
	mRepeatFrame(0)
{
	mSign = nullptr;
}
Animation::Animation(const sf::Texture& texture)
	: mSprite(texture)
	, mFrameSize()
	, mNumFrames(0)
	, mCurrentFrame(0)
	, mDuration(sf::Time::Zero)
	, mElapsedTime(sf::Time::Zero)
	, mRepeat(false),
	textureOffset(0, 0),
	hasVectorFrames(false)
{
	mSign = nullptr;
}
void Animation::update(sf::Time dt) {
	//updates the position of the texture rect for the animation
	//moves down vertically from start picture to bottom end picture
	sf::Time timePerFrame = mDuration / static_cast<float>(mNumFrames);
	mElapsedTime += dt;

	sf::Vector2i textureBounds(mSprite.getTexture()->getSize());
	sf::IntRect textureRect = mSprite.getTextureRect();

	if (mCurrentFrame == 0 && !hasVectorFrames) {
		textureRect = sf::IntRect(textureOffset.x, textureOffset.y, mFrameSize.x, mFrameSize.y); //added texture offsets
		textureRect = flip(textureRect);
	}
	else if (mCurrentFrame == 0 && hasVectorFrames) {
		textureRect = vecFrames[0];
		textureRect = flip(textureRect);
	}
	while (mElapsedTime >= timePerFrame && (mCurrentFrame < mNumFrames || mRepeat)) {
		mElapsedTime -= timePerFrame;
		if (mRepeat) {
			mCurrentFrame = (mCurrentFrame + 1) % (mNumFrames); //resets to 0 when reached last frame
			if (mCurrentFrame == 0) 						//reset the frame to start on either 0 frame, or the specified repeatframe to start
				mCurrentFrame = mRepeatFrame;
			textureRect.left = textureOffset.x + (mFrameSize.x * mCurrentFrame); //adds option to skip first frames when looping, skips by mReapeatFrame amount
			
			if (hasVectorFrames) {
				textureRect = vecFrames[mCurrentFrame];
				textureRect = flip(textureRect);
			}
		}
		else {
			if (hasVectorFrames) {
				textureRect = vecFrames[mCurrentFrame];
				textureRect = flip(textureRect);
			}
			mCurrentFrame++;
			if(!hasVectorFrames)
				textureRect.left = textureOffset.x + (mFrameSize.x * mCurrentFrame); //move left by currentFrame amount
		}
		//flip if facing left
		if (!hasVectorFrames)
			textureRect = flip(textureRect);
	}
	mSprite.setTextureRect(textureRect);
	centerOrigin(mSprite); //This line replaces the need to centerOrigin(Animation);
	//mSprite.scale(-1.f, 1.f); //tried to flip, created two ghost images keeping for future use
}
sf::IntRect Animation::flip(sf::IntRect textureRect) {
	if (mSign != nullptr && *mSign < 0) {
		textureRect.left += hasVectorFrames ? vecFrames[mCurrentFrame].width : mFrameSize.x;
		textureRect.width = hasVectorFrames ? vecFrames[mCurrentFrame].width * int(*mSign) : mFrameSize.x * int(*mSign);

	}
	else {

		textureRect.width = hasVectorFrames ? vecFrames[mCurrentFrame].width : mFrameSize.x;
	}
	return textureRect;
}

void Animation::setVectorFrames(vector<sf::IntRect> frames) {
	vecFrames = frames;
	hasVectorFrames = true;
}
void Animation::setRepeatOn(int frame) {
	mRepeatFrame = frame; //initially zero
}
void Animation::reset() {
	mCurrentFrame = 0;
	mElapsedTime = sf::Time::Zero;
}
void Animation::setTextureOffset(int x, int y) {
	textureOffset.x = x;
	textureOffset.y = y;
}
void Animation::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= getTransform();
	target.draw(mSprite, states);
}
void Animation::setTexture(const sf::Texture& texture)
{
	mSprite.setTexture(texture);
}

const sf::Texture* Animation::getTexture() const
{
	return mSprite.getTexture();
}

void Animation::setFrameSize(sf::Vector2i frameSize)
{
	mFrameSize = frameSize;
}

sf::Vector2i Animation::getFrameSize() const
{
	return mFrameSize;
}

void Animation::setNumFrames(std::size_t numFrames)
{
	mNumFrames = numFrames;
}

std::size_t Animation::getNumFrames() const
{
	return mNumFrames;
}

void Animation::setDuration(sf::Time duration)
{
	mDuration = duration;
}

sf::Time Animation::getDuration() const
{
	return mDuration;
}

void Animation::setRepeating(bool flag)
{
	mRepeat = flag;
}

bool Animation::isRepeating() const
{
	return mRepeat;
}

void Animation::restart()
{
	mCurrentFrame = 0;
}

bool Animation::isFinished() const
{
	return mCurrentFrame >= mNumFrames;
}

sf::FloatRect Animation::getLocalBounds() const
{
	return sf::FloatRect(getOrigin(), static_cast<sf::Vector2f>(getFrameSize()));
}

sf::FloatRect Animation::getGlobalBounds() const
{
	return getTransform().transformRect(getLocalBounds());
}
void Animation::setSign(float& sign) {
	mSign = &sign;
}