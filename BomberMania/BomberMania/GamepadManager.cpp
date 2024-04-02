#include "GamepadManager.h"
#include "Assets.h"
#include "Utilities.h"
#include <sstream>

GamepadManager& GamepadManager::getInstance()
{
	static GamepadManager gamepadManager;
	return gamepadManager;
}

void GamepadManager::handleEvent(const sf::Event& event)
{
	if (event.type == sf::Event::JoystickMoved)
	{
		std::string id = std::to_string(event.joystickMove.joystickId);

		// LEFT STICK
		actions[id + "_LS_UP"] = "END";
		actions[id + "_LS_DOWN"] = "END";
		actions[id + "_LS_LEFT"] = "END";
		actions[id + "_LS_RIGHT"] = "END";

		// DPAD
		actions[id + "_DPAD_UP"] = "END";
		actions[id + "_DPAD_DOWN"] = "END";
		actions[id + "_DPAD_LEFT"] = "END";
		actions[id + "_DPAD_RIGHT"] = "END";

		switch (event.joystickMove.axis)
		{
			// LEFT STICK
		case sf::Joystick::X:
			gamepad.ls.x = event.joystickMove.position;
			break;

		case sf::Joystick::Y:
			gamepad.ls.y = event.joystickMove.position;
			break;

			// DPAD
		case sf::Joystick::PovX:
			gamepad.dpad.x = event.joystickMove.position;
			break;

		case sf::Joystick::PovY:
			gamepad.dpad.y = event.joystickMove.position;
			break;
		}

		// LEFT STICK
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

		// DPAD
		if (std::fabsf(gamepad.dpad.x) > std::fabs(gamepad.dpad.y))
		{
			if (gamepad.dpad.x >= 15.0f) actions[id + "_DPAD_RIGHT"] = "START";
			else if (gamepad.dpad.x <= -15.0f) actions[id + "_DPAD_LEFT"] = "START";
		}
		else
		{
			if (gamepad.dpad.y >= 15.0f) actions[id + "_DPAD_DOWN"] = "START";
			else if (gamepad.dpad.y <= -15.0f) actions[id + "_DPAD_UP"] = "START";
		}
	}
	// adicionar botoes para pause usando o Start do controller
	else if (event.type == sf::Event::JoystickButtonPressed)
	{
		std::string id = std::to_string(event.joystickButton.joystickId);

		// Inicializa todas as ações como "END"
		actions[id + "_A"] = "END";
		actions[id + "_START"] = "END";
		actions[id + "_Y"] = "END";

		// Atualiza as ações conforme os botões pressionados
		switch (event.joystickButton.button)
		{
		case 0:
			actions[id + "_A"] = "START";
			break;

		case 7:
			actions[id + "_START"] = "START";
			break;

		case 3:
			actions[id + "_Y"] = "START";
			break;

			// Aqui você pode adicionar tratamento para outros botões, se necessário

		default:
			break;
		}
	}
	else if (event.type == sf::Event::JoystickButtonReleased)
	{
		std::string id = std::to_string(event.joystickButton.joystickId);

		// Atualiza as ações conforme os botões soltos
		switch (event.joystickButton.button)
		{
		case 0:
			actions[id + "_A"] = "END";
			break;

		case 7:
			actions[id + "_START"] = "END";
			break;

		case 3:
			actions[id + "_Y"] = "END";
			break;

		default:
			break;
		}
	}
}

void GamepadManager::drawDebugInfo(sf::RenderTarget* target)
{
	std::ostringstream oss;
	oss << "X Axis: " << gamepad.ls.x << '\n';
	oss << "Y Axis: " << gamepad.ls.y << '\n';
	oss << "PovX Axis: " << gamepad.dpad.x << '\n';
	oss << "PovY Axis: " << gamepad.dpad.y << '\n';

	//if (std::fabsf(gamepad.ls.x) > std::fabs(gamepad.ls.y)) // Eixo X é maior
	//{
	//	if (gamepad.ls.x >= 15.0f) oss << "LS_RIGHT" << '\n';
	//	else if (gamepad.ls.y <= -15.0f) oss << "LS_LEFT" << '\n';
	//}
	//else // Eixo Y maior
	//{
	//	if (gamepad.ls.x >= 15.0f) oss << "LS_DOWN" << '\n';
	//	else if (gamepad.ls.y <= -15.0f) oss << "LS_UP" << '\n';
	//}

	for (auto& [a, t] : actions)
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


// Exemplo de função interativa para descobrir os números dos botões
void GamepadManager::testJoystickButtons(sf::RenderWindow& window, unsigned int joystickId)
{
	sf::Joystick::update();
	if (sf::Joystick::isConnected(joystickId))
	{
		std::ostringstream oss;
		oss << "Joystick " << joystickId << " buttons: ";
		for (unsigned int i = 0; i < sf::Joystick::getButtonCount(joystickId); ++i)
		{
			if (sf::Joystick::isButtonPressed(joystickId, i))
			{
				oss << i << ' ';
			}
		}

		sf::Text text(oss.str(), Assets::getInstance().getFont("Arial"));
		text.setFillColor(sf::Color::Yellow);
		text.setPosition(10.0f, 10.0f);

		window.draw(text);
	}
}


GamepadActions GamepadManager::getActions() const
{
	return actions;
}