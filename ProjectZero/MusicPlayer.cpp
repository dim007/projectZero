#include "stdafx.h"
#include "MusicPlayer.h"

MusicPlayer::MusicPlayer()
	: mVolume(100.f),
	mFilenames(),
	mMusic()
{
	mFilenames[Music::MainMenu] = "Music/main_menu.ogg";
	mFilenames[Music::Level1] = "Music/mmX3_intro_stage.ogg";
}
void MusicPlayer::play(Music::ID theme) {
	std::string filename = mFilenames[theme];
	if (!mMusic.openFromFile(filename))
		throw std::runtime_error("Music " + filename + "could not be loaded.");

	mMusic.setVolume(mVolume);
	mMusic.setLoop(true);
	mMusic.play();
}
void MusicPlayer::stop() {
	mMusic.stop();
}
void MusicPlayer::setPaused(bool paused) {
	if (paused)
		mMusic.pause();
	else
		mMusic.play();
}
void MusicPlayer::setVolume(float volume) {
	mVolume = volume;
	mMusic.setVolume(mVolume);
}
void MusicPlayer::setSkip(sf::Time offset) {
	mMusic.setPlayingOffset(offset);
}
float MusicPlayer::getVolume() {
	return mVolume;
}