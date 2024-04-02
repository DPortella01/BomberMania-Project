#include "Scene_Menu.h"
#include "Scene_Bomb.h"
#include <memory>
#include "MusicPlayer.h"
#include "SoundPlayer.h"

void Scene_Menu::onEnd()
{
	m_game->window().close();
}

Scene_Menu::Scene_Menu(GameEngine* gameEngine)
	: Scene(gameEngine)
{
	init();
}



void Scene_Menu::init()
{
	registerAction(sf::Keyboard::W, "UP");
	registerAction(sf::Keyboard::Up, "UP");
	registerAction(sf::Keyboard::S, "DOWN");
	registerAction(sf::Keyboard::Down, "DOWN");
	registerAction(sf::Keyboard::D, "PLAY");
	registerAction(sf::Keyboard::Escape, "QUIT");

	m_menuStrings.push_back("Play");
	m_menuStrings.push_back("Options");
	m_menuStrings.push_back("How To Play");
	m_menuStrings.push_back("Quit");

	m_levelPaths.push_back("../assets/level1.txt");
	m_levelPaths.push_back("../assets/level1.txt");
	m_levelPaths.push_back("../assets/level1.txt");
	m_levelPaths.push_back("../assets/level1.txt");


	m_menuText.setFont(Assets::getInstance().getFont("ArcadeBomb"));

	const size_t CHAR_SIZE{ 64 };
	m_menuText.setCharacterSize(CHAR_SIZE);

	//add music to the menu
	MusicPlayer::getInstance().play("menuTheme");


}

void Scene_Menu::update(sf::Time dt)
{
	m_entityManager.update();
}


void Scene_Menu::sRender()
{
	// Configurar a visão
	sf::View view = m_game->window().getDefaultView();
	view.setCenter(m_game->window().getSize().x / 2.f, m_game->window().getSize().y / 2.f);

	m_game->window().setView(view);

	// Limpar o buffer de renderização
	m_game->window().clear();

	// Ler uma imagem de fundo
	sf::Texture backgroundImage;
	backgroundImage.loadFromFile("../assets/Textures/background1.png");

	sf::Sprite backgroundSprite(backgroundImage);

	// Escalar a imagem para preencher a tela
	backgroundSprite.setScale(
		static_cast<float>(m_game->window().getSize().x) / backgroundSprite.getLocalBounds().width,
		static_cast<float>(m_game->window().getSize().y) / backgroundSprite.getLocalBounds().height
	);

	// Desenhar o background
	m_game->window().draw(backgroundSprite);

	// Desenhar os elementos restantes
	m_menuText.setFillColor(sf::Color::Black);
	m_menuText.setString(m_title);
	m_menuText.setPosition(15, 15);
	m_game->window().draw(m_menuText);

	for (size_t i{ 0 }; i < m_menuStrings.size(); ++i)
	{
		//set the specific color of the text #FED001 
		if (i == m_menuIndex)
		{
			m_menuText.setFillColor(sf::Color(254, 208, 1));
		}
		else
		{
			m_menuText.setFillColor(sf::Color::Black);
		}
		//set the size of the text
		m_menuText.setCharacterSize(24);
		//justify the text to the center
		m_menuText.setPosition(370, 388 + i * 85);
		m_menuText.setString(m_menuStrings.at(i));
		m_game->window().draw(m_menuText);
	}

	sf::Text footer("UP: W    DOWN: S   PLAY:D    QUIT: ESC",
		Assets::getInstance().getFont("main"), 20);
	footer.setFillColor(sf::Color::Black);
	footer.setPosition(32, 700);
	m_game->window().draw(footer);

	// Exibir o que foi desenhado
	//m_game->window().display();
}




void Scene_Menu::sDoAction(const Command& action)
{
	if (action.type() == "START")
	{
		if (action.name() == "UP")
		{
			m_menuIndex = (m_menuIndex + m_menuStrings.size() - 1) % m_menuStrings.size();
			//setar som quando o menu muda
			SoundPlayer::getInstance().play("MenuSound");
		}
		else if (action.name() == "DOWN")
		{
			m_menuIndex = (m_menuIndex + 1) % m_menuStrings.size();
			SoundPlayer::getInstance().play("MenuSound");

		}
		else if (action.name() == "PLAY")
		{
			m_game->changeScene("PLAY", std::make_shared<Scene_Bomb>(m_game, m_levelPaths[m_menuIndex]));
			SoundPlayer::getInstance().play("ClickMenuSound");
		}
		else if (action.name() == "QUIT")
		{
			onEnd();
			SoundPlayer::getInstance().play("MenuSound");

		}
	}

}