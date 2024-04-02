#pragma once

#include <SFML/Graphics.hpp>
#include "Command.h"

using GamepadActions = std::map<std::string, std::string>;

class GamepadManager
{
private:
	struct Gamepad
	{
		sf::Vector2f ls;	// Left Stick
		sf::Vector2f dpad;
	};

	GamepadManager() = default;
	~GamepadManager() = default;

public:
	static GamepadManager& getInstance();

	GamepadManager(const GamepadManager&) = delete;
	GamepadManager(GamepadManager&&) = delete;
	GamepadManager& operator=(const GamepadManager&) = delete;
	GamepadManager& operator=(GamepadManager&&) = delete;

private:
	std::vector<std::string> debug;

	Gamepad gamepad;
	GamepadActions actions;

public:
	void handleEvent(const sf::Event& event);
	void drawDebugInfo(sf::RenderTarget* target);
	void testJoystickButtons(sf::RenderWindow& window, unsigned int joystickId);

	GamepadActions getActions() const;
};