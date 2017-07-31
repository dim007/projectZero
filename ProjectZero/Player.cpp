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
		playerRobot& robot = static_cast<playerRobot&>(node);
		if(!robot.airDashing)
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
	stopDash.category = Category::PlayerRobot;
	stopDash.action = derivedAction<playerRobot>([](playerRobot& a, sf::Time) { a.isDashing = false; a.airDashing = false; a.dashTime = sf::Time::Zero; a.canDash = false; });
	stopJump.category = Category::PlayerRobot;
	stopJump.action = derivedAction<playerRobot>([](playerRobot& a, sf::Time) { a.isJumping = false; });
}
void Player::initializeActions() {
	const float playerSpeed = Table[Robot::Zero].speed;
	//const float jumpSpeed = 800.f;

	mActionBinding[MoveLeft].action = derivedAction<playerRobot>(PlayerMover(-playerSpeed, 0.f));
	mActionBinding[MoveRight].action = derivedAction<playerRobot>(PlayerMover(+playerSpeed, 0.f));
	mActionBinding[MoveUp].action = derivedAction<playerRobot>(PlayerMover(0.f, -playerSpeed));
	mActionBinding[MoveDown].action = derivedAction<playerRobot>(PlayerMover(0.f, +playerSpeed));
	mActionBinding[Jump].action = derivedAction<playerRobot>([](playerRobot& a, sf::Time) { a.isJumping = true; });
	mActionBinding[Fire].action = derivedAction<playerRobot>([](playerRobot& a, sf::Time) { a.fire(); });
	mActionBinding[LaunchMissile].action = derivedAction<playerRobot>([](playerRobot& a, sf::Time) { a.launchMissile(); });
	mActionBinding[Slash].action = derivedAction<playerRobot>([](playerRobot& a, sf::Time) { if (a.slashing3 == false) { a.attackCount++; } });
	mActionBinding[Dash].action = derivedAction<playerRobot>([](playerRobot& a, sf::Time) { a.isDashing = true; });
}
void Player::handleEvent(const sf::Event& event, std::queue<Command>& commands) {
	if (event.type == sf::Event::JoystickButtonPressed) {
		if (sf::Joystick::isButtonPressed(2, 0) && !isRealtimeAction(Jump))
			commands.push(mActionBinding[Jump]);
		if (sf::Joystick::isButtonPressed(2, 2) && !isRealtimeAction(Slash))
			commands.push(mActionBinding[Slash]);
		if (sf::Joystick::isButtonPressed(2, 5) && !isRealtimeAction(Dash))
			commands.push(mActionBinding[Dash]);
	}
	else if (event.type == sf::Event::JoystickButtonReleased) {
		switch (event.joystickButton.button)
		{
		case 0:
			commands.push(stopJump);
		case 5:
			commands.push(stopDash);
		default:
			break;
		}
	}

	if (event.type == sf::Event::KeyPressed) {
		auto found = mKeyBinding.find(event.key.code);
		if (found != mKeyBinding.end() && !isRealtimeAction(found->second)) {
			commands.push(mActionBinding[found->second]);
		}
	}
		//Stop Dash
	else if (event.type == sf::Event::KeyReleased) {
		if (event.key.code == getAssignedKey(Action::Dash))
			commands.push(stopDash);
		if (event.key.code == getAssignedKey(Action::Jump))
			commands.push(stopJump);
	}
}
void Player::handleRealTimeInput(std::queue <Command> & commands) {
	//Joystick
	if (sf::Joystick::getAxisPosition(2, sf::Joystick::Axis::PovX) > 0)
		commands.push(mActionBinding[MoveRight]);
	else if (sf::Joystick::getAxisPosition(2, sf::Joystick::Axis::PovX) < 0)
		commands.push(mActionBinding[MoveLeft]);
	if (sf::Joystick::getAxisPosition(2, sf::Joystick::Axis::Z) > 0)
		commands.push(mActionBinding[Fire]);


	// Traverse all assigned keys and check if they are pressed
	for (auto pair : mKeyBinding)
	{
		// If key is pressed, lookup action and trigger corresponding command
		if (sf::Keyboard::isKeyPressed(pair.first) && isRealtimeAction(pair.second)) {
			commands.push(mActionBinding[pair.second]);
		}
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
	case Fire: //no dash or slash or jump
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
void Player::setLives(int lives) {
	mLives = lives;
}
int Player::getLives() {
	return mLives;
}