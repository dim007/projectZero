#include "stdafx.h"
#include "Platform.h"
#include "utilities.h"


//**** NOTE: Do Not instantiate SceneNode() in constructor, will create duplicate objects
//IntRect(left, top, width, height)
//top/left = 0 for creating a box starting at 0,0 on the sprite
Platform::Platform(Platform::Type type, TextureHolder& textures, sf::IntRect size)
	: mType(type),
	mSprite(textures.get(getTextureID()), size)
{
	textures.get(getTextureID()).setRepeated(true);
}
void Platform::drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const {
	target.draw(mSprite, states);
}
Textures::ID Platform::getTextureID() {
	switch (mType) {
	case Platform::Highway:
		return Textures::Highway;
	default:
		return Textures::Zero;
	}
}
unsigned int Platform::getCategory() const {
	return Category::Platform;
}
sf::FloatRect Platform::getBoundingRect() const {
	return getWorldTransform().transformRect(mSprite.getGlobalBounds());
}