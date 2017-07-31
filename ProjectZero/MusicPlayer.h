#pragma once
#include "stdafx.h"
#include "SFML\Audio.hpp"
#include <map>

namespace Music {
	enum ID {
		MainMenu,
		Level1
	};
}
class MusicPlayer : private sf::NonCopyable
{
public:
	MusicPlayer();
	void play(Music::ID theme);
	void stop();
	void setPaused(bool paused);
	void setVolume(float volume);
	void setSkip(sf::Time offset);
	float getVolume();
private:
	sf::Music mMusic;
	std::map<Music::ID, std::string> mFilenames;
	float mVolume;

};