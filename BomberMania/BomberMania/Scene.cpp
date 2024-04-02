#include "Scene.h"
#include "MusicPlayer.h"


Scene::Scene(GameEngine* gameEngine) : m_game(gameEngine)
{}

Scene::~Scene()
{}

void Scene::setPaused(bool paused)
{
	m_isPaused = paused;
}

void Scene::pause()
{
    // Alterna o estado de pausa
    m_isPaused = !m_isPaused;

    // Verifica se o jogo está pausado
    if (m_isPaused)
    {
        MusicPlayer::getInstance().stop();
    }
    else
    {
        // Se não estiver pausado, não há necessidade de limpar a tela com a cor cinza.
        // Você pode, opcionalmente, limpar a tela com a cor de fundo padrão do jogo aqui.

        MusicPlayer::getInstance().play("gameTheme");
    }
}


void Scene::simulate(int)
{}

void Scene::doAction(Command command)
{
	this->sDoAction(command);
}


const CommandMap Scene::getActionMap() const
{
	return m_commands;
}

const GamepadMap Scene::getGamepadMap() const
{
    return m_gamepadMap;
}

void Scene::registerAction(int inputKey, std::string command)
{
	m_commands[inputKey] = command;
}

void Scene::registerGamepadAction(std::string action, std::string command)
{
    m_gamepadMap[action] = command;
}