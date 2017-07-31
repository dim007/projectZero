#include "stdafx.h"
#include "Player.h"
#include <iostream>
using namespace std;

namespace {
	const std::vector<RobotData> Table = initializeRobotData(); //found in DataTables.h
}
struct PlayerMover {
	PlayerMover(float vx, float vy) : velocity(vx, vy) {}

	void operator() (SceneNode& node, sf::Time) const {
		Robot& robot = static_cast<Robot&>(node);
		robot.setVelocity(robot.getVelocity() + velocity);
	}
	sf::Vector2f velocity;
};

Player::Player() 
	: mCurrentMissionStatus(MissionRunning)
{
	//initial binds
	mKeyBinding[sf::Keyboard::A] = MoveLeft;
	mKeyBinding[sf::Keyboard::D] = MoveRight;
	mKeyBinding[sf::Keyboard::W] = MoveUp;
	mKeyBinding[sf::Keyboard::S] = MoveDown;
	mKeyBinding[sf::Keyboard::Space] = Jump;
	mKeyBinding[sf::Keyboard::Period] = Fire;
	mKeyBinding[sf::Keyboard::BackSlash] = LaunchMissile;
	mKeyBinding[sf::Keyboard::Comma] = Slash;
	mKeyBinding[sf::Keyboard::M] = Dash;
	//set keybindings
	initializeActions();

	//Assign the actions to player category
	for (auto itr = mActionBinding.begin(); itr != mActionBinding.end(); itr++) {
		itr->second.category = Category::PlayerRobot;
	}
	//stopDash command
	stopDash.category = Category::PlayerRobot;
	//stopDash.action = derivedAction<Robot>([](Robot& a, sf::Time) { a.stopDash(); });
}
void Player::initializeActions() {
	const float playerSpeed = Table[Robot::Zero].speed;
	//const float jumpSpeed = 800.f;

	mActionBinding[MoveLeft].action = derivedAction<Robot>(PlayerMover(-playerSpeed, 0.f));
	mActionBinding[MoveRight].action = derivedAction<Robot>(PlayerMover(+playerSpeed, 0.f));
	mActionBinding[MoveUp].action = derivedAction<Robot>(PlayerMover(0.f, -playerSpeed));
	mActionBinding[MoveDown].action = derivedAction<Robot>(PlayerMover(0.f, +playerSpeed));
	mActionBinding[Jump].action = derivedAction<Robot>([](Robot& a, sf::Time) {a.jump(); });
	mActionBinding[Fire].action = derivedAction<Robot>([](Robot& a, sf::Time) { a.fire(); });
	mActionBinding[LaunchMissile].action = derivedAction<Robot>([](Robot& a, sf::Time) { a.launchMissile(); });
//	mActionBinding[Slash].action = derivedAction<Robot>([] (Robot& a, sf::Time) { a.slash(); });
	//mActionBinding[Dash].action = derivedAction<Robot>([](Robot& a, sf::Time) { a.dash(); });
}
void Player::handleEvent(const sf::Event& event, std::queue<Command>& commands) {
	if (event.type == sf::Event::KeyReleased) {
		auto found = mKeyBinding.find(event.key.code);
		if (found != mKeyBinding.end() && !isRealtimeAction(found->second))
			commands.push(mActionBinding[found->second]);
		
		//Stop Dash
		//if (event.key.code == getAssignedKey(Action::Dash)) {
			//commands.push(stopDash);
		//}
	}
}
void Player::handleRealTimeInput(std::queue <Command> & commands) {
	// Traverse all assigned keys and check if they are pressed
	for (auto pair : mKeyBinding)
	{
		// If key is pressed, lookup action and trigger corresponding command
		if (sf::Keyboard::isKeyPressed(pair.first) && isRealtimeAction(pair.second))
			commands.push(mActionBinding[pair.second]);
	}
	/*const float playerSpeed = 100.f;
	const float jumpSpeed = 500.f;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
		Command moveLeft;
		moveLeft.category = Category::PlayerRobot;
		moveLeft.action = derivedAction<Robot>(PlayerMover(-playerSpeed, 0.f));
		commands.push(moveLeft);
	}*/
}
void Player::assignKey(Action action, sf::Keyboard::Key key) {
	// Remove all keys that already map to action
	for (auto itr = mKeyBinding.begin(); itr != mKeyBinding.end(); )
	{
		if (itr->second == action)
			mKeyBinding.erase(itr++);
		else
			++itr;
	}
	mKeyBinding[key] = action; //add new pair to map
}
sf::Keyboard::Key Player::getAssignedKey(Action action) const{
	for (auto pair : mKeyBinding) {
		if (pair.second == action)
			return pair.first;
	}
	return sf::Keyboard::Unknown;
}

bool Player::isRealtimeAction(Action action)
{
	switch (action)
	{
	case MoveLeft:
	case MoveRight:
	case MoveDown:
	case MoveUp:
	case Jump:
	case Fire:
	case Slash:
	case Dash:
		return true;

	default:
		return false;
	}
}
Player::MissionStatus Player::getMissionStatus() const {
	return mCurrentMissionStatus;
}
void Player::setMissionStatus(MissionStatus status) {
	mCurrentMissionStatus = status;
}