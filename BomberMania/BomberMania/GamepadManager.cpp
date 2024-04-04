#include "GamepadManager.h"
#include "Assets.h"
#include "Utilities.h"
#include <sstream>

GamepadManager::GamepadManager()
{
	gamepads.resize(sf::Joystick::Count);
}

GamepadManager& GamepadManager::getInstance()
{
	static GamepadManager gamepadManager;
	return gamepadManager;
}

void GamepadManager::handleEvent(const sf::Event& event)
{
	if (event.type == sf::Event::JoystickMoved)
	{
		auto& gamepad = gamepads[event.joystickMove.joystickId];
		std::string id = std::to_string(event.joystickMove.joystickId);

		// LEFT STICK
		if (event.joystickMove.axis == sf::Joystick::X ||
			event.joystickMove.axis == sf::Joystick::Y)
		{
			actions[id + "_LS_UP"] = "END";
			actions[id + "_LS_DOWN"] = "END";
			actions[id + "_LS_LEFT"] = "END";
			actions[id + "_LS_RIGHT"] = "END";

			switch (event.joystickMove.axis)
			{
			case sf::Joystick::X:
				gamepad.ls.x = event.joystickMove.position;
				break;

			case sf::Joystick::Y:
				gamepad.ls.y = event.joystickMove.position;
				break;
			}

			if (std::fabsf(gamepad.ls.x) > std::fabs(gamepad.ls.y))
			{
				if (gamepad.ls.x >= 15.0f) actions[id + "_LS_RIGHT"] = "START";
				else if (gamepad.ls.x <= -15.0f) actions[id + "_LS_LEFT"] = "START";
			}
			else
			{
				if (gamepad.ls.y >= 15.0f) actions[id + "_LS_DOWN"] = "START";
				else if (gamepad.ls.y <= -15.0f) actions[id + "_LS_UP"] = "START";
			}
		}

		// DPAD
		if (event.joystickMove.axis == sf::Joystick::PovX ||
			event.joystickMove.axis == sf::Joystick::PovY)
		{
			actions[id + "_DPAD_UP"] = "END";
			actions[id + "_DPAD_DOWN"] = "END";
			actions[id + "_DPAD_LEFT"] = "END";
			actions[id + "_DPAD_RIGHT"] = "END";

			switch (event.joystickMove.axis)
			{
			case sf::Joystick::PovX:
				gamepad.dpad.x = event.joystickMove.position;
				break;

			case sf::Joystick::PovY:
				gamepad.dpad.y = event.joystickMove.position;
				break;
			}

			if (std::fabsf(gamepad.dpad.x) > std::fabs(gamepad.dpad.y))
			{
				if (gamepad.dpad.x >= 15.0f) actions[id + "_DPAD_RIGHT"] = "START";
				else if (gamepad.dpad.x <= -15.0f) actions[id + "_DPAD_LEFT"] = "START";
			}
			else
			{
				if (gamepad.dpad.y >= 15.0f) actions[id + "_DPAD_UP"] = "START";
				else if (gamepad.dpad.y <= -15.0f) actions[id + "_DPAD_DOWN"] = "START";
			}
		}
	}

	if (event.type == sf::Event::JoystickButtonPressed || event.type == sf::Event::JoystickButtonReleased)
	{
		std::string id = std::to_string(event.joystickButton.joystickId);
		std::string button = std::to_string(event.joystickButton.button);

		actions[id + "_B" + button] = event.type == sf::Event::JoystickButtonPressed ? "START" : "END";
	}
}

void GamepadManager::drawDebugInfo(sf::RenderTarget* target)
{
	auto& gamepad = gamepads[0];

	std::ostringstream oss;
	oss << "X Axis: " << gamepad.ls.x << '\n';
	oss << "Y Axis: " << gamepad.ls.y << '\n';
	oss << "PovX Axis: " << gamepad.dpad.x << '\n';
	oss << "PovY Axis: " << gamepad.dpad.y << '\n';

	for (auto& [a, t] : debugActions)
	{
		oss << a << ':' << t << '\n';
	}

	sf::Text text(oss.str(), Assets::getInstance().getFont("Arial"));
	text.setFillColor(sf::Color::Yellow);
	text.setPosition(10.0f, 10.0f);

	auto& view = target->getDefaultView();
	sf::Vector2f pos(150.0f, view.getSize().y - 150.0f);

	sf::CircleShape oc(100.0f);
	oc.setFillColor(sf::Color::Transparent);
	oc.setOutlineColor(sf::Color::Yellow);
	oc.setOutlineThickness(5.0f);
	centerOrigin(oc);
	oc.setPosition(pos);

	sf::CircleShape ic(25.0f);
	ic.setFillColor(sf::Color::Red);
	centerOrigin(ic);
	ic.setPosition(pos.x + gamepad.ls.x, pos.y + gamepad.ls.y);

	target->setView(view);
	target->draw(text);
	target->draw(oc);
	target->draw(ic);
}

GamepadActions GamepadManager::getActions() const
{
	return actions;
}

void GamepadManager::clearActions()
{
	// For apenas pro drawDebugInfo
	for (auto& [action, type] : actions)
	{
		debugActions[action] = type;
	}

	actions.clear();
}