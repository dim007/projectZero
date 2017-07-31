#ifndef TITLESTATE
#define TITLESTATE
#include "stdafx.h"
#include "State.h"

class TitleState : public State {
public:
	TitleState(StateStack& stack, Context context, StatesID::ID stateID);
	void draw();
	bool update(sf::Time dt);
	bool handleEvent(const sf::Event& event);
private:
	//sf::Sprite mBackgroundSprite;
	sf::Text mTitleText1;
	sf::Text mTitleText2;
	sf::Text mText;

	bool mShowText;
	sf::Time mTextEffectTime;
};

#endif // !TITLESTATE