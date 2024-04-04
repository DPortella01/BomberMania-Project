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

	GamepadManager();
	~GamepadManager() = default;

public:
	static GamepadManager& getInstance();

	GamepadManager(const GamepadManager&) = delete;
	GamepadManager(GamepadManager&&) = delete;
	GamepadManager& operator=(const GamepadManager&) = delete;
	GamepadManager& operator=(GamepadManager&&) = delete;

private:
	std::vector<Gamepad> gamepads;
	GamepadActions actions;
	GamepadActions debugActions;

public:
	void handleEvent(const sf::Event& event);
	void drawDebugInfo(sf::RenderTarget* target);

	GamepadActions getActions() const;
	void clearActions();
};