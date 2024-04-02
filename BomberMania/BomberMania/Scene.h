#pragma once

#include "EntityManager.h"
#include "GameEngine.h"
#include "Command.h"
#include <map>
#include <string>


using CommandMap = std::map<int, std::string>;
using GamepadMap = std::map<std::string, std::string>;

class Scene_Bomb;

class Scene
{

protected:

	GameEngine*			m_game;
	EntityManager		m_entityManager;
	CommandMap			m_commands;
	GamepadMap			m_gamepadMap;
	bool				m_isPaused{ false };
	bool				m_hasEnded{ false };
	bool                m_isWon{ false };
	size_t				m_currentFrame{ 0 };

	virtual void		onEnd() = 0;
	void				setPaused(bool paused);
	void				pause();

public:
	Scene(GameEngine* gameEngine);
	virtual ~Scene();

	virtual void		update(sf::Time dt) = 0;
	virtual void		sDoAction(const Command& action) = 0;
	virtual void		sRender() = 0;

	void				simulate(int);
	void				doAction(Command);
	void				registerAction(int, std::string);
	void				registerGamepadAction(std::string, std::string);
	const CommandMap	getActionMap() const;
	const GamepadMap	getGamepadMap() const;
};

