#ifndef STATE
#define STATE
#include "stdafx.h"
#include <memory> //for unique ptr
#include "ResourceHolder.h"
#include "TextureID.h"
#include "SFML\Graphics.hpp"
#include "MusicPlayer.h"

namespace sf {
	class RenderWindow;
}
namespace StatesID {
	enum ID {
		None,
		Title,
		Menu,
		Game,
		Loading,
		Settings,
		Pause,
		GameOver
	};
}
//Bellow both needed to act as empty unitintialized class. StateStack.h included later
//in State.cpp
class Player;
class StateStack;

class State {
public:
	typedef std::unique_ptr<State> Ptr;
	typedef ResourceHolder<sf::Texture, Textures::ID> TextureHolder; //shortcut for typing
	typedef ResourceHolder<sf::Font, Fonts::ID> FontHolder;

	//context allows all states to share the same resources
	//**loaded in Application.cpp and Context passed to all States
	struct Context
	{
		Context(sf::RenderWindow& window,
			TextureHolder& textures,
			FontHolder& fonts,
			Player& player,
			MusicPlayer& musicplayer);

		sf::RenderWindow* window;
		TextureHolder* textures;
		FontHolder* fonts;
		Player* player;
		MusicPlayer* musicplayer;
	};
public:
	State(StateStack& stack, Context context, StatesID::ID stateID);
	virtual ~State();

	StatesID::ID mStateID;
	virtual void draw() = 0;
	virtual bool update(sf::Time dt) = 0;
	virtual bool handleEvent(const sf::Event& event) = 0;

protected:
	void requestStackPush(StatesID::ID stateID);
	void requestStackPop();
	void requestStateClear();

	Context getContext() const;
private:
	StateStack* mStack;
	Context mContext;
};

#endif