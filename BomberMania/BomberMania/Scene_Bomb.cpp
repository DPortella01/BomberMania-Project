//
// Created by David Burchill on 2023-09-27.
//

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

	sf::Vector2f spawnPos{ m_worldView.getSize().x / 2.f, m_worldBounds.height - m_worldView.getSize().y / 2.f };
	m_worldView.setCenter(spawnPos);
	spawnPlayer(spawnPos);

	MusicPlayer::getInstance().play("gameTheme");
	MusicPlayer::getInstance().setVolume(5);
}


void Scene_Bomb::init(const std::string& path) {
}

void Scene_Bomb::sMovement(sf::Time dt) {
	playerMovement();

	// move all objects
	for (auto e : m_entityManager.getEntities()) {
		if (e->hasComponent<CTransform>()) {
			auto& tfm = e->getComponent<CTransform>();

			tfm.pos += tfm.vel * dt.asSeconds();
			tfm.angle += tfm.angVel * dt.asSeconds();
		}
	}
}


void Scene_Bomb::registerActions() {
	registerAction(sf::Keyboard::P, "PAUSE");
	registerAction(sf::Keyboard::Escape, "BACK");
	registerAction(sf::Keyboard::Q, "QUIT");
	registerAction(sf::Keyboard::C, "TOGGLE_COLLISION");

	registerAction(sf::Keyboard::A, "LEFT");
	registerAction(sf::Keyboard::Left, "LEFT");
	registerAction(sf::Keyboard::D, "RIGHT");
	registerAction(sf::Keyboard::Right, "RIGHT");
	registerAction(sf::Keyboard::W, "UP");
	registerAction(sf::Keyboard::Up, "UP");
	registerAction(sf::Keyboard::S, "DOWN");
	registerAction(sf::Keyboard::Down, "DOWN");

	registerAction(sf::Keyboard::Space, "FIRE");
	registerAction(sf::Keyboard::M, "LAUNCH");
}


void Scene_Bomb::onEnd() {
	m_game->changeScene("MENU", nullptr, false);
}

void Scene_Bomb::playerMovement() {

	// no movement if player is dead
	if (m_player->hasComponent<CState>() && m_player->getComponent<CState>().state == "dead")
		return;

	// player movement
	sf::Vector2f pv{ 0.f,0.f };
	auto& pInput = m_player->getComponent<CInput>();

	//This part is used to change the texture of the player when the left and right arrow keys are pressed
	if (pInput.up) {
		pv.y -= 1;

		auto& animation = m_player->getComponent<CAnimation>().animation;

		if (animation.getName() != "up")
			animation = m_player->addComponent<CAnimation>(Assets::getInstance().getAnimation("up")).animation;

		animation.play();
	}
	if (pInput.down) {
		pv.y += 1;

		auto& animation = m_player->getComponent<CAnimation>().animation;

		if (animation.getName() != "down")
			animation = m_player->addComponent<CAnimation>(Assets::getInstance().getAnimation("down")).animation;

		animation.play();
	}
	if (pInput.left) {
		pv.x -= 1;

		auto& animation = m_player->getComponent<CAnimation>().animation;

		if (animation.getName() != "left")
			animation = m_player->addComponent<CAnimation>(Assets::getInstance().getAnimation("left")).animation;

		animation.play();
	}
	if (pInput.right) {
		pv.x += 1;

		auto& animation = m_player->getComponent<CAnimation>().animation;

		if (animation.getName() != "right")
			animation = m_player->addComponent<CAnimation>(Assets::getInstance().getAnimation("right")).animation;

		animation.play();
	}

	/*
	else {
		// Optionally, reset the sprite to the default when the left arrow key is not pressed
		auto& sprite = m_player->getComponent<CSprite>().sprite;
		auto [txtName, txtRect] = Assets::getInstance().getSprt("EagleStr");
		sprite.setTexture(Assets::getInstance().getTexture(txtName));
		sprite.setTextureRect(txtRect);
		centerOrigin(sprite);
	}

	if (pInput.up) pv.y -= 1;
	if (pInput.down) pv.y += 1;
	*/

	if (pv.x == 0 && pv.y == 0)
		m_player->getComponent<CAnimation>().animation.stop();

	pv = normalize(pv);
	m_player->getComponent<CTransform>().vel = m_config.playerSpeed * pv;

}


void Scene_Bomb::sRender() {
	//m_game->window().clear(sf::Color(100, 100, 255));
	m_game->window().setView(m_worldView);

	for (auto& e : m_entityManager.getEntities()) {
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
}


void Scene_Bomb::update(sf::Time dt) {
	sUpdate(dt);
}

void Scene_Bomb::sDoAction(const Command& action) {
	// On Key Press
	if (action.type() == "START") {
		if (action.name() == "PAUSE") { setPaused(!m_isPaused); }
		else if (action.name() == "QUIT") { m_game->quitLevel(); }
		else if (action.name() == "BACK") { m_game->backLevel(); }

		else if (action.name() == "TOGGLE_TEXTURE") { m_drawTextures = !m_drawTextures; }
		else if (action.name() == "TOGGLE_COLLISION") { m_drawAABB = !m_drawAABB; }
		else if (action.name() == "TOGGLE_GRID") { m_drawGrid = !m_drawGrid; }

		// Player control
		else if (action.name() == "LEFT") { m_player->getComponent<CInput>().left = true; }
		else if (action.name() == "RIGHT") { m_player->getComponent<CInput>().right = true; }
		else if (action.name() == "UP") { m_player->getComponent<CInput>().up = true; }
		else if (action.name() == "DOWN") { m_player->getComponent<CInput>().down = true; }
	}

	// on Key Release
	else if (action.type() == "END") {
		if (action.name() == "LEFT") { m_player->getComponent<CInput>().left = false; }
		else if (action.name() == "RIGHT") { m_player->getComponent<CInput>().right = false; }
		else if (action.name() == "UP") { m_player->getComponent<CInput>().up = false; }
		else if (action.name() == "DOWN") { m_player->getComponent<CInput>().down = false; }
	}
}


void Scene_Bomb::spawnPlayer(sf::Vector2f pos) {
	m_player = m_entityManager.addEntity("player");
	m_player->addComponent<CTransform>(pos);
	m_player->addComponent<CState>("straight");
	m_player->addComponent<CBoundingBox>(sf::Vector2f(16.0f, 16.0f), sf::Vector2f(0.0f, 8.0f));
	m_player->addComponent<CInput>();

	m_player->addComponent<CAnimation>(Assets::getInstance().getAnimation("down"));

	/*
	auto& sprite = m_player->addComponent<CSprite>().sprite;
	auto [txtName, txtRect] = Assets::getInstance().getSprt("BombermanDown");
	sprite.setTexture(Assets::getInstance().getTexture(txtName));
	sprite.setTextureRect(txtRect);
	centerOrigin(sprite);
	*/
}


sf::FloatRect Scene_Bomb::getViewBounds() {
	auto view = m_game->window().getView();
	return sf::FloatRect(
		(view.getCenter().x - view.getSize().x / 2.f), (view.getCenter().y - view.getSize().y / 2.f),
		view.getSize().x, view.getSize().y);
}


void Scene_Bomb::sCollisions() {
	adjustPlayerPosition();

	m_player->getComponent<CAnimation>().animation.m_sprite.setColor(sf::Color::White);

	for (auto& tile : m_entityManager.getEntities("Tile"))
	{
		if (tile->hasComponent<CBoundingBox>()) // Se o tile collide (se tem o yes ou no)
		{
			auto overlap = Physics::getOverlap(m_player, tile);
			if (overlap.x > 0 && overlap.y > 0) // se overlpar em x e y for maior que zero, ele entrou no tile
			{
				m_player->getComponent<CAnimation>().animation.m_sprite.setColor(sf::Color::Red);

				// Aqui dentro faz o código pro player não andar.
				// Usar o overlap
			}

			int a = 10;
		}
	}
}


void Scene_Bomb::sUpdate(sf::Time dt) {
	SoundPlayer::getInstance().removeStoppedSounds();

	if (m_isPaused)
		return;

	m_entityManager.update();
	//m_worldView.move(0.f, m_config.scrollSpeed * dt.asSeconds() * -1);

	SoundPlayer::getInstance().removeStoppedSounds();
	adjustPlayerPosition();
	//checkPlayerState();
	// move everything
	sAnimation(dt);
	sMovement(dt); //  movement first,
	sCollisions(); //  deal with collisions after movement.
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

	auto& player_pos = m_player->getComponent<CTransform>().pos;
	auto halfSize = m_player->getComponent<CBoundingBox>().halfSize;
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
			std::string collide;
			config >> name >> pos.x >> pos.y >> rect.left >> rect.top >> rect.width >> rect.height >> collide;
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
		}
		/*else if (token == "World") {
			config >> m_worldBounds.width >> m_worldBounds.height;
		}*/
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
