#ifndef PLAYER
#define PLAYER
#include "stdafx.h"
#include <queue>
#include "Command.h" 
#include <SFML/Graphics.hpp> //needed for sf::Event
#include "playerRobot.h"
#include "SpriteNode.h"

class Player {
public:
	enum Action {
		MoveLeft,
		MoveRight,
		MoveUp,
		MoveDown,
		Fire,
		LaunchMissile,
		Jump,
		Slash,
		Dash,
		ActionCount
	};
	enum MissionStatus {
		MissionRunning,
		MissionSuccess,
		MissionFailure
	};
public:
	Player();

	void handleEvent(const sf::Event& event, std::queue<Command>& commands);
	void handleRealTimeInput(std::queue<Command>& commands);

	void assignKey(Action action, sf::Keyboard::Key key);
	sf::Keyboard::Key getAssignedKey(Action action) const;

	void setMissionStatus(MissionStatus status);
	MissionStatus getMissionStatus() const;

	int getLives();
	void setLives(int lives);
private:
	void initializeActions();
	static bool isRealtimeAction(Action action);

private:
	Command stopDash;
	Command stopJump;

	std::map<sf::Keyboard::Key, Action> mKeyBinding;
	std::map<Action, Command> mActionBinding;
	MissionStatus mCurrentMissionStatus;
	int mLives;
};

#endif