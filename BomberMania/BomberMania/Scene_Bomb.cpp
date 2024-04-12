#include <fstream>
#include <iostream>

#include "Scene_Bomb.h"
#include "Components.h"
#include "Physics.h"
#include "Utilities.h"
#include "MusicPlayer.h"
#include "Assets.h"
#include "SoundPlayer.h"
#include <random>

namespace {
	std::random_device rd;
	std::mt19937 rng(rd());
}

Scene_Bomb::Scene_Bomb(GameEngine* gameEngine, const std::string& levelPath)
	: Scene(gameEngine)
	, m_worldView(gameEngine->window().getDefaultView()) {
	loadLevel(levelPath);
	registerActions();

	m_worldView = sf::View(m_worldBounds);

	//Initiate the time with 180 seconds
	m_time = sf::seconds(180);

	sf::Vector2f spawnPos(48, 32);
	spawnPlayer(m_player1, spawnPos, "SpriteSheet");

	spawnPos = sf::Vector2f(224, 176);
	spawnPlayer(m_player2, spawnPos, "SpriteSheet2");

	MusicPlayer::getInstance().play("gameTheme");
	MusicPlayer::getInstance().setVolume(20);
}

void Scene_Bomb::init()
{

}

void Scene_Bomb::sMovement(sf::Time dt) {
	for (auto& e : m_entityManager.getEntities("player"))
		playerMovement(e);

	// move all objects
	for (auto e : m_entityManager.getEntities()) {
		if (e->getComponent<CState>().state == "dead")
			return;

		if (e->hasComponent<CTransform>()) {
			auto& tfm = e->getComponent<CTransform>();
			auto& pup = e->getComponent<CPlayer>();

			tfm.prevPos = tfm.pos;								// Salva a posição atual como anterior (antiga).
			tfm.pos += tfm.vel * dt.asSeconds() * pup.speed;    // Atualiza a posição atual para a nova.
		}
	}
}

void Scene_Bomb::registerActions() {
	registerAction(sf::Keyboard::P, "PAUSE");
	registerAction(sf::Keyboard::Escape, "BACK");
	registerAction(sf::Keyboard::Q, "QUIT");
	registerAction(sf::Keyboard::C, "TOGGLE_COLLISION");
	registerAction(sf::Keyboard::Enter, "GAMEQUIT");

	registerAction(sf::Keyboard::A, "LEFT");
	registerAction(sf::Keyboard::Left, "LEFT2");
	registerAction(sf::Keyboard::D, "RIGHT");
	registerAction(sf::Keyboard::Right, "RIGHT2");
	registerAction(sf::Keyboard::W, "UP");
	registerAction(sf::Keyboard::Up, "UP2");
	registerAction(sf::Keyboard::S, "DOWN");
	registerAction(sf::Keyboard::Down, "DOWN2");

	registerAction(sf::Keyboard::Space, "BOMB");
	registerAction(sf::Keyboard::Numpad0, "BOMB2");

	registerGamepadAction("0_LS_UP", "UP");
	registerGamepadAction("0_LS_DOWN", "DOWN");
	registerGamepadAction("0_LS_LEFT", "LEFT");
	registerGamepadAction("0_LS_RIGHT", "RIGHT");

	registerGamepadAction("0_DPAD_UP", "UP");
	registerGamepadAction("0_DPAD_DOWN", "DOWN");
	registerGamepadAction("0_DPAD_LEFT", "LEFT");
	registerGamepadAction("0_DPAD_RIGHT", "RIGHT");

	registerGamepadAction("0_B0", "BOMB"); // X
	registerGamepadAction("0_B3", "GAMEQUIT"); // Y
	registerGamepadAction("0_B7", "BACK"); // START

	registerGamepadAction("1_LS_UP", "UP2");
	registerGamepadAction("1_LS_DOWN", "DOWN2");
	registerGamepadAction("1_LS_LEFT", "LEFT2");
	registerGamepadAction("1_LS_RIGHT", "RIGHT2");

	registerGamepadAction("1_DPAD_UP", "UP2");
	registerGamepadAction("1_DPAD_DOWN", "DOWN2");
	registerGamepadAction("1_DPAD_LEFT", "LEFT2");
	registerGamepadAction("1_DPAD_RIGHT", "RIGHT2");

	registerGamepadAction("1_B0", "BOMB2"); // X
	registerGamepadAction("1_B3", "GAMEQUIT"); // Y
	registerGamepadAction("1_B7", "BACK"); // START
}

void Scene_Bomb::onEnd() {
	m_game->changeScene("MENU", nullptr, false);
}

void Scene_Bomb::playerMovement(sPtrEntt& playerPtr) {
	// no movement if player is dead
	if (playerPtr->hasComponent<CState>() && playerPtr->getComponent<CState>().state == "dead")
		return;

	// player movement
	sf::Vector2f pv{ 0.f,0.f };
	auto& pInput = playerPtr->getComponent<CInput>();

	auto& sprite = playerPtr->getComponent<CPlayer>().sprite;

	//This part is used to change the texture of the player when the left and right arrow keys are pressed
	if (pInput.up) {
		pv.y -= 1;

		auto& animation = playerPtr->getComponent<CAnimation>().animation;

		if (animation.getName() != "up")
		{
			animation = playerPtr->addComponent<CAnimation>(Assets::getInstance().getAnimation("up")).animation;
			animation.changeTexture(sprite);
		}

		animation.play();
	}
	else if (pInput.down) {
		pv.y += 1;

		auto& animation = playerPtr->getComponent<CAnimation>().animation;

		if (animation.getName() != "down")
		{
			animation = playerPtr->addComponent<CAnimation>(Assets::getInstance().getAnimation("down")).animation;
			animation.changeTexture(sprite);
		}

		animation.play();
	}
	else if (pInput.left) {
		pv.x -= 1;

		auto& animation = playerPtr->getComponent<CAnimation>().animation;

		if (animation.getName() != "left")
		{
			animation = playerPtr->addComponent<CAnimation>(Assets::getInstance().getAnimation("left")).animation;
			animation.changeTexture(sprite);
		}

		animation.play();
	}
	else if (pInput.right) {
		pv.x += 1;

		auto& animation = playerPtr->getComponent<CAnimation>().animation;

		if (animation.getName() != "right")
		{
			animation = playerPtr->addComponent<CAnimation>(Assets::getInstance().getAnimation("right")).animation;
			animation.changeTexture(sprite);
		}

		animation.play();
	}

	if (pv.x == 0 && pv.y == 0)
		playerPtr->getComponent<CAnimation>().animation.stop();

	pv = normalize(pv);
	playerPtr->getComponent<CTransform>().vel = m_config.playerSpeed * pv;
}

void Scene_Bomb::renderEntity(std::shared_ptr<Entity>& e)
{
	if (e->hasComponent<CSprite>())
	{
		auto& sprite = e->getComponent<CSprite>().sprite;
		if (e->hasComponent<CTransform>()) {
			auto& tfm = e->getComponent<CTransform>();
			//sprite.setPosition(tfm.pos);

		}
		m_game->window().draw(sprite);
	}

	if (e->hasComponent<CAnimation>())
	{
		auto& anim = e->getComponent<CAnimation>().animation;
		auto& tfm = e->getComponent<CTransform>();
		anim.getSprite().setPosition(tfm.pos);
		anim.getSprite().setRotation(tfm.angle);
		m_game->window().draw(anim.getSprite());
	}

	if (m_drawAABB) {
		if (e->hasComponent<CBoundingBox>()) {
			auto box = e->getComponent<CBoundingBox>();
			sf::RectangleShape rect;
			rect.setSize(sf::Vector2f{ box.size.x, box.size.y });
			centerOrigin(rect);
			rect.setPosition(e->getComponent<CTransform>().pos + box.offset);
			rect.setFillColor(sf::Color(0, 0, 0, 0));
			rect.setOutlineColor(sf::Color{ 0, 255, 0 });
			rect.setOutlineThickness(0.5f);
			m_game->window().draw(rect);
		}
	}
}

void Scene_Bomb::executeMenuAction(size_t selectedOption) {
	switch (selectedOption) {
	case 0: // Play
		// release the paused game
		m_isPaused = false;
		break;
	case 1: // Options
		//Open the options menu
		break;
	case 2: // How to Play
		// Open game instructions
		break;
	case 3: // Quit
		// Quit the game
		m_game->quitLevel();
		break;
	default:
		// Invalid option
		break;
	}
}

void Scene_Bomb::sRender() {
	m_game->window().clear();
	m_game->window().setView(m_worldView); // defines the zoom


	for (auto& e : m_entityManager.getEntities())
	{
		if (e->getTag() == "player") continue;	// skips the player.
		if (e->getTag() == "Fire") continue;

		renderEntity(e);	// drawing the players and the bomb.
	}

	auto& fires = m_entityManager.getEntities("Fire");

	for (int i = fires.size(); i > 0; i--)
	{
		renderEntity(fires[i - 1]);
	}

	// draw the plzyers at the end and on top of everything.
	for (auto& e : m_entityManager.getEntities("player"))
	{
		renderEntity(e);
	}

	m_game->window().setView(m_game->window().getDefaultView());

	///////////////////////////////////////////////////////////
	//TIMER

	sf::Text text;
	text.setFont(Assets::getInstance().getFont("ArcadeBomb"));
	text.setCharacterSize(24);
	text.setFillColor(sf::Color(254, 208, 1));
	text.setOutlineColor(sf::Color::Black);
	text.setOutlineThickness(1);
	text.setPosition(740, 10);
	text.setString("TIME: " + std::to_string(static_cast<int>(m_time.asSeconds())));

	m_game->window().draw(text);

	///////////////////////////////////////////////////////////
	//SCORE

	// Create and configure the text to display player 1's score
	sf::Text scoreText;
	scoreText.setFont(Assets::getInstance().getFont("ArcadeBomb"));
	scoreText.setCharacterSize(24);
	scoreText.setFillColor(sf::Color::White);
	scoreText.setOutlineColor(sf::Color::Black);
	scoreText.setOutlineThickness(1);
	scoreText.setPosition(20, 10);

	//Update the content of the text to display the current score of player 1
	if (m_player1) {
		scoreText.setString("Player 1: " + std::to_string(m_player1->getComponent<CScore>().score));
	}
	else {
		scoreText.setString("Player 1: 0");
	}

	//Render the text of player 1's score
	m_game->window().draw(scoreText);

	//Create and configure the text to display player 2's score
	sf::Text scoreText2;
	scoreText2.setFont(Assets::getInstance().getFont("ArcadeBomb"));
	scoreText2.setCharacterSize(24);
	scoreText2.setFillColor(sf::Color::White);
	scoreText2.setOutlineColor(sf::Color::Black);
	scoreText2.setOutlineThickness(1);
	scoreText2.setPosition(700, 720);

	// Update the content of the text to display the current score of player 2
	if (m_player2) {
		scoreText2.setString("Player 2: " + std::to_string(m_player2->getComponent<CScore>().score));
	}
	else {
		scoreText2.setString("Player 2: 0");
	}

	//Render the text of player 2's score
	m_game->window().draw(scoreText2);

	/////////////////////////////////////////////////////////////
	////VICTORY SCREEN
	if (m_finished) {
		drawVictoryScreen();
		//add a victory song
	}


	///////////////////////////////////////////////////////////
	//MENU PAUSE

	if (m_isPaused) {
		sf::RectangleShape grayScreen(sf::Vector2f(m_game->window().getSize()));
		grayScreen.setFillColor(sf::Color(24, 24, 24, 200));
		m_game->window().draw(grayScreen);

		sf::Texture pauseTexture;
		pauseTexture.loadFromFile("../assets/Textures/123.png");

		sf::Sprite pauseSprite(pauseTexture);

		float scaleX = static_cast<float>(m_game->window().getSize().x) / (pauseSprite.getLocalBounds().width);
		float scaleY = static_cast<float>(m_game->window().getSize().y) / (pauseSprite.getLocalBounds().height);

		float scaleFactor = std::min(scaleX, scaleY);

		float maxScaleFactor = 0.25f;
		scaleFactor = std::min(scaleFactor, maxScaleFactor);

		pauseSprite.setScale(scaleFactor, scaleFactor);

		float posX = (m_game->window().getSize().x - pauseSprite.getGlobalBounds().width) / 2.0f;
		float posY = (m_game->window().getSize().y - pauseSprite.getGlobalBounds().height) / 2.0f;

		posX += m_game->window().getView().getCenter().x - m_game->window().getSize().x / 2.0f;
		posY += m_game->window().getView().getCenter().y - m_game->window().getSize().y / 2.0f;

		pauseSprite.setPosition(posX, posY);

		m_game->window().draw(pauseSprite);


		//////////////////////////////////////////////////////

		static bool prevUpPressed = false;
		static bool prevDownPressed = false;

		sf::Text menuText;
		menuText.setFont(Assets::getInstance().getFont("ArcadeBomb"));
		menuText.setCharacterSize(18);
		menuText.setFillColor(sf::Color::White);

		float textScale = 0.30f;
		menuText.setScale(sf::Vector2f(textScale, textScale));

		sf::Vector2f baseTextPosition(
			m_game->window().getView().getCenter().x - menuText.getLocalBounds().width / 2.0f * textScale - 30.0f,
			m_game->window().getView().getCenter().y - menuText.getLocalBounds().height / 2.0f * textScale - -7.0f
		);

		std::vector<std::string> menuStrings = {
			"RESUME",
			"OPTIONS",
			"HOW TO PLAY",
			"QUIT"
		};

		size_t selectedOption = 0;

		bool upPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && !prevUpPressed;

		bool downPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Down) && !prevDownPressed;

		if (upPressed) {
			if (selectedOption > 0) {
				--selectedOption;
			}
		}

		if (downPressed) {
			if (selectedOption < menuStrings.size() - 1) {
				++selectedOption;
			}
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
			executeMenuAction(selectedOption);
		}

		for (size_t i = 0; i < menuStrings.size(); ++i)
		{
			menuText.setString(menuStrings[i]);

			menuText.setPosition(
				baseTextPosition.x,
				baseTextPosition.y + i * menuText.getLocalBounds().height * 4.2f * textScale
			);

			if (i == selectedOption) {
				menuText.setFillColor(sf::Color::Yellow);
			}
			else {
				menuText.setFillColor(sf::Color::White);
			}

			m_game->window().draw(menuText);
		}
	}
}

void Scene_Bomb::drawVictoryScreen()
{
	sf::RectangleShape grayScreen(sf::Vector2f(m_game->window().getSize()));
	grayScreen.setFillColor(sf::Color(24, 24, 24, 200));
	m_game->window().draw(grayScreen);

	sf::Texture victoryTexture;
	victoryTexture.loadFromFile("../assets/Textures/victory.png");

	sf::Sprite victorySprite(victoryTexture);

	victorySprite.setScale(0.7f, 0.7f);

	victorySprite.setPosition(
		(m_game->window().getSize().x - victorySprite.getGlobalBounds().width) / 2.0f,
		(m_game->window().getSize().y - victorySprite.getGlobalBounds().height) / 2.0f - 100
	);

	m_game->window().draw(victorySprite);

	sf::Text victoryText;
	victoryText.setFont(Assets::getInstance().getFont("ArcadeBomb"));
	victoryText.setCharacterSize(56);
	victoryText.setFillColor(sf::Color(210, 46, 56));
	victoryText.setOutlineColor(sf::Color::Black);
	victoryText.setOutlineThickness(1);

	if (m_player1->getComponent<CState>().state != "dead" && m_player2->getComponent<CState>().state == "dead") {
		victoryText.setString("Player 1 Wins!");
	}
	else if (m_player2->getComponent<CState>().state != "dead" && m_player1->getComponent<CState>().state == "dead") {
		victoryText.setString("Player 2 Wins!");
	}
	else if (m_player1->getComponent<CState>().state == "dead" && m_player2->getComponent<CState>().state == "dead") {
		victoryText.setString("Draw!");
	}
	else if (m_player1->getComponent<CScore>().score > m_player2->getComponent<CScore>().score) {
		victoryText.setString("Player 1 Wins!");
	}
	else if (m_player1->getComponent<CScore>().score < m_player2->getComponent<CScore>().score) {
		victoryText.setString("Player 2 Wins!");
	}
	else
	{
		victoryText.setString("Draw!");
	}

	victoryText.setPosition(
		(m_game->window().getSize().x - victoryText.getLocalBounds().width) / 2.0f,
		(m_game->window().getSize().y - victoryText.getLocalBounds().height) / 2.0 - -50
	);

	sf::Text menuText;
	menuText.setFont(Assets::getInstance().getFont("ArcadeBomb"));
	menuText.setCharacterSize(30);
	menuText.setFillColor(sf::Color::White);
	menuText.setOutlineColor(sf::Color::Black);
	menuText.setOutlineThickness(1);

	std::vector<std::string> menuStrings = {
		"Play Again: A",
		"Quit: Y"
	};

	for (size_t i = 0; i < menuStrings.size(); ++i)
	{
		menuText.setString(menuStrings[i]);
		menuText.setPosition(
			(m_game->window().getSize().x - menuText.getLocalBounds().width) / 2.0f,
			(m_game->window().getSize().y - menuText.getLocalBounds().height) / 2.0f + 220.0f + i * menuText.getLocalBounds().height * 2.2f
		);


		menuText.setFillColor(sf::Color::Yellow);

		m_game->window().draw(menuText);
	}

	m_game->window().draw(victoryText);
}

void Scene_Bomb::update(sf::Time dt) {
	sUpdate(dt);
}

void Scene_Bomb::sDoAction(const Command& action) {
	// On Key Press
	if (action.type() == "START") {
		if (action.name() == "PAUSE") { pause(); }
		else if (action.name() == "QUIT") { m_game->quitLevel(); }
		else if (action.name() == "BACK") { m_game->backLevel(); }

		else if (action.name() == "TOGGLE_TEXTURE") { m_drawTextures = !m_drawTextures; }
		else if (action.name() == "TOGGLE_COLLISION") { m_drawAABB = !m_drawAABB; }
		else if (action.name() == "TOGGLE_GRID") { m_drawGrid = !m_drawGrid; }

		// Player1 control
		else if (action.name() == "LEFT") { m_player1->getComponent<CInput>().left = true; }
		else if (action.name() == "RIGHT") { m_player1->getComponent<CInput>().right = true; }
		else if (action.name() == "UP") { m_player1->getComponent<CInput>().up = true; }
		else if (action.name() == "DOWN") { m_player1->getComponent<CInput>().down = true; }

		else if (action.name() == "BOMB") {
			if (m_finished)
			{
				m_game->resetLevel();
				SoundPlayer::getInstance().play("ClickMenuSound");
			}
			else
			{
				dropBomb(m_player1);
			}
		}

		else if (action.name() == "GAMEQUIT")
		{
			if (m_finished)
			{
				m_game->quit();
				SoundPlayer::getInstance().play("ClickMenuSound");
			}
		}

		// Player2 control
		else if (action.name() == "LEFT2") { m_player2->getComponent<CInput>().left = true; }
		else if (action.name() == "RIGHT2") { m_player2->getComponent<CInput>().right = true; }
		else if (action.name() == "UP2") { m_player2->getComponent<CInput>().up = true; }
		else if (action.name() == "DOWN2") { m_player2->getComponent<CInput>().down = true; }

		else if (action.name() == "BOMB2") { dropBomb(m_player2); }
	}

	// on Key Release
	else if (action.type() == "END") {
		if (action.name() == "LEFT") { m_player1->getComponent<CInput>().left = false; }
		else if (action.name() == "RIGHT") { m_player1->getComponent<CInput>().right = false; }
		else if (action.name() == "UP") { m_player1->getComponent<CInput>().up = false; }
		else if (action.name() == "DOWN") { m_player1->getComponent<CInput>().down = false; }

		if (action.name() == "LEFT2") { m_player2->getComponent<CInput>().left = false; }
		else if (action.name() == "RIGHT2") { m_player2->getComponent<CInput>().right = false; }
		else if (action.name() == "UP2") { m_player2->getComponent<CInput>().up = false; }
		else if (action.name() == "DOWN2") { m_player2->getComponent<CInput>().down = false; }
	}
}

//Add the new parameter to define which sprite will be used
void Scene_Bomb::spawnPlayer(sPtrEntt& playerPtr, sf::Vector2f pos, const std::string& sprite) {
	playerPtr = m_entityManager.addEntity("player");
	playerPtr->addComponent<CTransform>(pos);
	playerPtr->addComponent<CBoundingBox>(sf::Vector2f(15.0f, 15.0f), sf::Vector2f(0.0f, 8.0f));
	playerPtr->addComponent<CScore>();
	playerPtr->addComponent<CInput>();
	playerPtr->addComponent<CPlayer>().sprite = sprite;

	auto& animation = playerPtr->addComponent<CAnimation>(Assets::getInstance().getAnimation("down")).animation;
	animation.changeTexture(sprite);
}

void Scene_Bomb::dropBomb(sPtrEntt& player)
{
	auto& pos = player->getComponent<CTransform>().pos;

	//transform pixel into grid
	int x = (int)(pos.x / 16);
	int y = (int)((pos.y + 8) / 16); // +8 to adjust the bomb position because the origin is in the center.

	// Tranformar grid em pixel
	sf::Vector2f np((x * 16) + 8, y * 16); // O +8 in x is to adjust the bomb position because the origin is in the center.

	auto& powerUp = player->getComponent<CPlayer>();

	// if the total of bombs is greater than the bombs dropped, the player can drop a bomb
	if (powerUp.bomb > powerUp.droped)
	{
		for (auto& e : m_entityManager.getEntities("Bomb"))
		{
			if (e->getComponent<CBomb>().isOnBomb == true)
				return;
		}

		spawnBomb(player, np);
		powerUp.droped++;
	}
}

void Scene_Bomb::spawnBomb(sPtrEntt& player, sf::Vector2f pos)
{
	pos.y += 8;

	auto bomb = m_entityManager.addEntity("Bomb");
	bomb->addComponent<CAnimation>(Assets::getInstance().getAnimation("bomb"));
	bomb->addComponent<CTransform>(pos);
	bomb->addComponent<CBoundingBox>(sf::Vector2f(16, 16));
	SoundPlayer::getInstance().play("PlaceBomb");
	bomb->addComponent<CBomb>();
	bomb->addComponent<COwner>(player);

	//de o play na animacao
	bomb->getComponent<CAnimation>().animation.play();
}

bool Scene_Bomb::isColliding(sf::Vector2f pos) {
	//verify if the explosion collides with the destructable bricks, non-destructable bricks or walls
	for (auto& e : m_entityManager.getEntities("Tile")) {
		if (e->hasComponent<CBoundingBox>()) {
			auto& tile = e->getComponent<CTransform>().pos;
			if (pos == tile) {
				auto tileType = e->getComponent<CTile>().type;
				//If colliding with a destructable brick, the brick is destroyed
				if (tileType == TileType::Destructable || tileType == TileType::None) {
					return true;
				}
			}
		}
	}
	for (auto& e : m_entityManager.getEntities("Bomb")) {
		if (e->hasComponent<CBoundingBox>()) {
			auto& tile = e->getComponent<CTransform>().pos;
			if (pos == tile) {
				e->getComponent<CBomb>().lifespan = sf::Time::Zero;
				return true;
			}
		}
	}
	return false;
}

void Scene_Bomb::destroyDestructableTile(sf::Vector2f pos, sPtrEntt& player) {
	for (auto& e : m_entityManager.getEntities("Tile")) {
		if (e->hasComponent<CBoundingBox>()) {
			auto& tile = e->getComponent<CTransform>().pos;
			if (pos == tile) {
				auto tileType = e->getComponent<CTile>().type;
				//If colliding with a destructable brick, the brick is destroyed
				if (tileType == TileType::Destructable) {
					e->removeComponent<CSprite>();
					e->addComponent<CAnimation>(Assets::getInstance().getAnimation("brick")).animation.play();

					player->getComponent<CScore>().score += 100;

					std::uniform_int_distribution percentage(0, 100);
					if (percentage(rng) <= 15)
					{
						std::uniform_int_distribution powerUpGenerator(1, 3);
						int powerUp = powerUpGenerator(rng);

						if (powerUp == 1 && m_fire > 0)
						{
							spawnPowerUp(tile, "powerUp1");
							m_fire--;
						}

						if (powerUp == 2 && m_bomb > 0)
						{
							spawnPowerUp(tile, "powerUp2");
							m_bomb--;
						}

						if (powerUp == 3 && m_speed > 0)
						{
							spawnPowerUp(tile, "powerUp3");
							m_speed--;
						}
					}
				}
			}
		}
	}
}

void Scene_Bomb::spawnExplosion(sPtrEntt& bomb)
{
	sf::Vector2f pos = bomb->getComponent<CTransform>().pos;
	auto& player = bomb->getComponent<COwner>().owner;
	int size = player->getComponent<CPlayer>().fire;

	//FIRE MIDDLE (fireM)
	spawnFire(pos, "fireM", player);

	sf::Vector2f nPos = pos;
	//FIRE LEFT
	for (int i = 0; i < size; i++)
	{
		nPos.x -= 16;
		if (isColliding(nPos))
		{
			destroyDestructableTile(nPos, player);
			break;
		}
		if (i == size - 1)
			spawnFire(nPos, "fireL", player); //FireL
		else
			spawnFire(nPos, "fireW", player); //FireW
	}

	//FIRE RIGHT
	nPos = pos; //center the position before going right
	for (int i = 0; i < size; i++)
	{
		nPos.x += 16;
		if (isColliding(nPos))
		{
			destroyDestructableTile(nPos, player);
			break;
		}
		if (i == size - 1)
			spawnFire(nPos, "fireR", player); // FireR
		else
			spawnFire(nPos, "fireW", player); // FireW
	}

	//FIRE UP
	nPos = pos; //center the position before going up
	for (int i = 0; i < size; i++)
	{
		nPos.y -= 16;
		if (isColliding(nPos))
		{
			destroyDestructableTile(nPos, player);
			break;
		}
		if (i == size - 1)
			spawnFire(nPos, "fireU", player); // FireU
		else
			spawnFire(nPos, "fireH", player); // FireH

	}

	//FIRE DOWN
	nPos = pos; //center the position before going down
	for (int i = 0; i < size; i++)
	{
		nPos.y += 16;
		if (isColliding(nPos))
		{
			destroyDestructableTile(nPos, player);
			break;
		}
		if (i == size - 1)
			spawnFire(nPos, "fireD", player); // FireD
		else
			spawnFire(nPos, "fireH", player); // FireH
	}
}

void Scene_Bomb::spawnFire(sf::Vector2f pos, const std::string& animation, sPtrEntt& player)
{
	auto e = m_entityManager.addEntity("Fire");
	auto& anim = e->addComponent<CAnimation>(Assets::getInstance().getAnimation(animation));
	e->addComponent<CTransform>(pos);
	e->addComponent<CBoundingBox>(sf::Vector2f(14, 14));
	e->addComponent<COwner>(player);

	SoundPlayer::getInstance().play("BombExplodes");

	anim.animation.play();
}

sf::FloatRect Scene_Bomb::getViewBounds() {
	auto view = m_game->window().getView();
	return sf::FloatRect(
		(view.getCenter().x - view.getSize().x / 2.f), (view.getCenter().y - view.getSize().y / 2.f),
		view.getSize().x, view.getSize().y);
}

void Scene_Bomb::spawnPowerUp(sf::Vector2f pos, const std::string& type)
{
	auto e = m_entityManager.addEntity("PowerUp");
	e->addComponent<CTransform>(pos);
	e->addComponent<CAnimation>(Assets::getInstance().getAnimation(type));
	e->addComponent<CBoundingBox>(sf::Vector2f(16, 16));

	e->getComponent<CAnimation>().animation.play();
}

void Scene_Bomb::spawnBrick(sf::Vector2f pos)
{
	auto e = m_entityManager.addEntity("Brick");
	e->addComponent<CTransform>(pos);
	e->addComponent<CAnimation>(Assets::getInstance().getAnimation("brick"));
	e->addComponent<CBoundingBox>(sf::Vector2f(16, 16));

	e->getComponent<CAnimation>().animation.play();
}

void Scene_Bomb::spawnDeath(sf::Vector2f pos)
{
	auto e = m_entityManager.addEntity("Death");
	e->addComponent<CTransform>(pos);
	e->addComponent<CAnimation>(Assets::getInstance().getAnimation("death"));
	e->addComponent<CBoundingBox>(sf::Vector2f(16, 16));

	e->getComponent<CAnimation>().animation.play();
}

void Scene_Bomb::sCollisions(sPtrEntt& playerPtr) {
	adjustPlayerPosition();

	//PLAYER AND TILE COLLISION
	for (auto& tile : m_entityManager.getEntities("Tile"))
	{
		if (tile->hasComponent<CBoundingBox>()) // if the tile has a bounding box
		{
			auto overlap = Physics::getOverlap(playerPtr, tile);
			auto prevOverlap = Physics::getPreviousOverlap(playerPtr, tile);

			if (overlap.x > 0 && overlap.y > 0) // if overlap is greater than 0, there is a collision
			{

				if (prevOverlap.x > 0) // if entered from the top or bottom, adjust the player's position vertically
				{
					auto& pos = playerPtr->getComponent<CTransform>().pos;

					if (overlap.x < 7.5f) // 7.5 related to the size of the cube
					{
						if (pos.x > tile->getComponent<CTransform>().pos.x) // entered the block from the right
							if (overlap.x - 0.5f < 0)
							{
								pos.x += overlap.x;
							}
							else
							{
								pos.x += 0.5f;
							}
						//pos.x += overlap.x - 2.0f < 0 ? overlap.x : 2.0f;
						else // entered the block from the left
							if (overlap.x - 0.5f < 0)
							{
								pos.x -= overlap.x;
							}
							else
							{
								pos.x -= 0.5f;
							}
						//pos.x -= overlap.x - 0.5f < 0 ? overlap.x : 0.5f;
					}

					if (pos.y > tile->getComponent<CTransform>().pos.y) // entered the block from the bottom
						pos.y += overlap.y;
					else // entered the block from the top
						pos.y -= overlap.y;
				}
				else // if entered from the left or right, adjust the player's position horizontally
				{
					auto& pos = playerPtr->getComponent<CTransform>().pos;

					if (overlap.y < 7.5f) // 7.5 related to the size of the cube
					{
						if (pos.y > tile->getComponent<CTransform>().pos.y) // entered the block from the bottom
							if (overlap.y - 0.5f < 0)
							{
								pos.y += overlap.y;
							}
							else
							{
								pos.y += 0.5f;
							}
						//pos.y += overlap.y - 0.5f < 0 ? overlap.y : 0.5f;
						else // entered the block from the top
							if (overlap.y - 0.5f < 0)
							{
								pos.y -= overlap.y;
							}
							else
							{
								pos.y -= 0.5f;
							}
						//pos.y -= overlap.y - 0.5f < 0 ? overlap.y : 0.5f;
					}

					if (pos.x > tile->getComponent<CTransform>().pos.x) // entrou no bloco pela direita
						pos.x += overlap.x;
					else // entered the block from the left
						pos.x -= overlap.x;
				}
			}
		}
	}

	// PLAYER AND BOMB COLLISION
	for (auto& bomb : m_entityManager.getEntities("Bomb"))
	{
		//auto& bombPos = bomb->getComponent<CTransform>().pos;
		auto overlap = Physics::getOverlap(playerPtr, bomb);
		auto prevOverlap = Physics::getPreviousOverlap(playerPtr, bomb);


		// verify if the player is on the bomb
		auto& isOnBomb = bomb->getComponent<CBomb>().isOnBomb;
		if (isOnBomb)
		{
			auto& bombPlayer = bomb->getComponent<COwner>().owner; // BombPlayer é o player que soltou essa bomba.
			auto overlapBP = Physics::getOverlap(bombPlayer, bomb); // Overlap do Bomb Player.

			// Verify if the player is still on the bomb, and then change the state of the bomb
			if (overlapBP.x < 0 || overlapBP.y < 0)
			{
				isOnBomb = false;
			}
		}
		else
		{
			// He is no longer on the bomb;
			//Verify if there was a collision and if the player was previously on the bomb
			if (overlap.x > 0 && overlap.y > 0 /* && wasOnBomb */) {
				//Adjust the player's position to avoid collision
				if (prevOverlap.x > 0)
				{
					//collision more strong in the Y axis, move the player vertically
					auto& pos = playerPtr->getComponent<CTransform>().pos;

					if (pos.y > bomb->getComponent<CTransform>().pos.y) // entered the block from the bottom
						pos.y += overlap.y;
					else // entered the block from the top
						pos.y -= overlap.y;
				}
				else
				{
					// Collision stronger in the X axis, move the player horizontally
					auto& pos = playerPtr->getComponent<CTransform>().pos;

					if (pos.x > bomb->getComponent<CTransform>().pos.x) // Entered the block from the right
						pos.x += overlap.x;
					else // Entered the block from the left
						pos.x -= overlap.x;
				}
			}
		}
	}

	// PLAYER AND POWERUP
	// TODO
	for (auto& powerUp : m_entityManager.getEntities("PowerUp"))
	{
		auto overlap = Physics::getOverlap(playerPtr, powerUp);
		auto prevOverlap = Physics::getPreviousOverlap(playerPtr, powerUp);

		if (overlap.x > 0 && overlap.y > 0)
		{
			auto& powerUpType = powerUp->getComponent<CAnimation>().animation.getName();
			if (powerUpType == "powerUp1")
			{
				playerPtr->getComponent<CPlayer>().fire++;
				SoundPlayer::getInstance().play("PowerUp");
			}
			else if (powerUpType == "powerUp2")
			{
				playerPtr->getComponent<CPlayer>().bomb++;
				SoundPlayer::getInstance().play("PowerUp");
			}
			else if (powerUpType == "powerUp3")
			{
				playerPtr->getComponent<CPlayer>().speed += 0.2f;
				SoundPlayer::getInstance().play("PowerUp");
			}
			powerUp->destroy();
		}
	}

	//POWERUP AND FIRE
	//TODO
	for (auto& powerUp : m_entityManager.getEntities("PowerUp"))
	{
		for (auto& fire : m_entityManager.getEntities("Fire"))
		{
			auto overlap = Physics::getOverlap(powerUp, fire);
			auto prevOverlap = Physics::getPreviousOverlap(powerUp, fire);

			if (overlap.x > 0 && overlap.y > 0)
			{
				powerUp->destroy();
			}
		}
	}

	// FIRE AND PLAYER
	for (auto& fire : m_entityManager.getEntities("Fire"))
	{
		auto overlap = Physics::getOverlap(playerPtr, fire);
		auto prevOverlap = Physics::getPreviousOverlap(playerPtr, fire);

		if (overlap.x > 0 && overlap.y > 0)
		{

			if (playerPtr->getComponent<CState>().state != "dead")
			{
				auto& animation = playerPtr->addComponent<CAnimation>(Assets::getInstance().getAnimation("death")).animation;
				animation.changeTexture(playerPtr->getComponent<CPlayer>().sprite);
				animation.play();

				SoundPlayer::getInstance().play("Audience");

				MusicPlayer::getInstance().play("victoryTheme");
				MusicPlayer::getInstance().setVolume(20);

				playerPtr->addComponent<CState>("dead");
			}
		}
	}
}

void Scene_Bomb::sUpdate(sf::Time dt) {
	SoundPlayer::getInstance().removeStoppedSounds();

	if (m_isPaused || m_finished)
		return;
	//m_worldView.move(0.f, m_config.scrollSpeed * dt.asSeconds() * -1);

	//Initiates the m_time with 180 seconds and decrements every second
	m_time -= dt;

	if (m_time <= sf::Time::Zero)
	{
		m_finished = true;
		m_time = sf::Time::Zero;
	}

	m_entityManager.update();

	SoundPlayer::getInstance().removeStoppedSounds();
	adjustPlayerPosition();
	//checkPlayerState();
	// move everything
	sAnimation(dt);
	sMovement(dt); //  movement first,

	for (auto& e : m_entityManager.getEntities("player"))
	{
		sCollisions(e);
	}

	sBombUpdate(dt);
}

void Scene_Bomb::sBombUpdate(sf::Time dt)
{
	for (auto& e : m_entityManager.getEntities("Bomb"))
	{
		auto& lifespan = e->getComponent<CBomb>().lifespan;
		lifespan -= dt;

		if (lifespan <= sf::Time::Zero)
		{
			e->getComponent<COwner>().owner->getComponent<CPlayer>().droped--; // diminui a contagem de bombas no mapa
			e->destroy();
			//auto& player = e->getComponent<COwner>().owner;
			spawnExplosion(e);
		}
	}
}

void Scene_Bomb::sAnimation(sf::Time dt)
{
	auto list = m_entityManager.getEntities();
	for (auto e : m_entityManager.getEntities()) {
		// update all animations
		if (e->hasComponent<CAnimation>()) {
			auto& anim = e->getComponent<CAnimation>();
			anim.animation.update(dt);
			// do nothing if animation has ended

			if (e->getComponent<CTile>().type == TileType::Destructable)
			{
				if (anim.animation.hasEnded())
					e->destroy();
			}

			if (e->getComponent<CState>().state == "dead")
			{
				if (anim.animation.hasEnded())
				{
					e->destroy();
					m_finished = true;
				}
			}
		}
	}
}

void Scene_Bomb::adjustPlayerPosition() {
	auto center = m_worldView.getCenter();
	sf::Vector2f viewHalfSize = m_worldView.getSize() / 2.f;

	auto left = center.x - viewHalfSize.x;
	auto right = center.x + viewHalfSize.x;
	auto top = center.y - viewHalfSize.y;
	auto bot = center.y + viewHalfSize.y;

	auto& player_pos = m_player1->getComponent<CTransform>().pos;
	auto halfSize = m_player1->getComponent<CBoundingBox>().halfSize;
	// keep player in bounds
	player_pos.x = std::max(player_pos.x, left + halfSize.x);
	player_pos.x = std::min(player_pos.x, right - halfSize.x);
	player_pos.y = std::max(player_pos.y, top + halfSize.y);
	player_pos.y = std::min(player_pos.y, bot - halfSize.y);
}

void Scene_Bomb::loadLevel(const std::string& path) {
	std::ifstream config(path);
	if (config.fail()) {
		std::cerr << "Open file " << path << " failed\n";
		config.close();
		exit(1);
	}

	std::string token{ "" };
	config >> token;
	while (!config.eof()) {
		if (token == "Bkg") {
			std::string name;
			sf::Vector2f pos;
			config >> name >> pos.x >> pos.y;
			auto e = m_entityManager.addEntity("bkg");

			// for background, no textureRect its just the whole texture
			// and no center origin, position by top left corner
			// stationary so no CTransfrom required.
			auto& sprite = e->addComponent<CSprite>(Assets::getInstance().getTexture(name)).sprite;
			sprite.setOrigin(0.f, 0.f);
			sprite.setPosition(pos);
		}
		else if (token == "Tile") {
			std::string name; // nome da textura do tile
			sf::Vector2f pos; // Position (posição do tile)
			sf::IntRect rect; // Rectangle (recorte do tile)
			std::string collide, type;
			config >> name >> pos.x >> pos.y >> rect.left >> rect.top >> rect.width >> rect.height >> collide >> type;
			auto e = m_entityManager.addEntity("Tile");

			// for background, no textureRect its just the whole texture
			// and no center origin, position by top left corner
			// stationary so no CTransfrom required.
			auto& sprite = e->addComponent<CSprite>(Assets::getInstance().getTexture(name), rect).sprite;
			sprite.setTextureRect(rect);

			pos.x += (rect.width / 2.0f);
			pos.y += (rect.height / 2.0f);

			sprite.setPosition(pos);

			e->addComponent<CTransform>(pos);

			if (collide == "yes")
				e->addComponent<CBoundingBox>(sf::Vector2f(16.0f, 16.0f));

			if (type == "destructable")
				e->addComponent<CTile>(TileType::Destructable);
			else
				e->addComponent<CTile>(TileType::None);
		}
		else if (token == "World") {
			config >> m_worldBounds.width >> m_worldBounds.height;
		}
		else if (token == "ScrollSpeed") {
			config >> m_config.scrollSpeed;
		}
		else if (token == "PlayerSpeed") {
			config >> m_config.playerSpeed;
		}
		else if (token[0] == '#') {
			std::string tmp;
			std::getline(config, tmp);
			std::cout << tmp << "\n";
		}
		config >> token;
	}

	config.close();
}

bool Scene_Bomb::checkPlayerVictory()
{
	return m_player1->getComponent<CState>().state == "dead" ||
		m_player2->getComponent<CState>().state == "dead" ||
		m_time <= sf::Time::Zero;
}