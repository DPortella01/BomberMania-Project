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

	sf::Vector2f spawnPos(48, 32);
	spawnPlayer(m_player1, spawnPos);

	spawnPos = sf::Vector2f(224, 176);
	spawnPlayer(m_player2, spawnPos);

	MusicPlayer::getInstance().play("gameTheme");
	MusicPlayer::getInstance().setVolume(5);
}


void Scene_Bomb::init(const std::string& path) {
}

void Scene_Bomb::sMovement(sf::Time dt) {
	for (auto& e : m_entityManager.getEntities("player"))
		playerMovement(e);

	// move all objects
	for (auto e : m_entityManager.getEntities()) {
		if (e->hasComponent<CTransform>()) {
			auto& tfm = e->getComponent<CTransform>();

			tfm.prevPos = tfm.pos;					// Salva a posição atual como anterior (antiga).
			tfm.pos += tfm.vel * dt.asSeconds();	// Atualiza a posição atual para a nova.
		}
	}
}


void Scene_Bomb::registerActions() {
	registerAction(sf::Keyboard::P, "PAUSE");
	registerAction(sf::Keyboard::Escape, "BACK");
	registerAction(sf::Keyboard::Q, "QUIT");
	registerAction(sf::Keyboard::C, "TOGGLE_COLLISION");

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

	//This part is used to change the texture of the player when the left and right arrow keys are pressed
	if (pInput.up) {
		pv.y -= 1;

		auto& animation = playerPtr->getComponent<CAnimation>().animation;

		if (animation.getName() != "up")
		{
			animation = playerPtr->addComponent<CAnimation>(Assets::getInstance().getAnimation("up")).animation;
		}


		animation.play();
	}
	if (pInput.down) {
		pv.y += 1;

		auto& animation = playerPtr->getComponent<CAnimation>().animation;

		if (animation.getName() != "down")
		{
			animation = playerPtr->addComponent<CAnimation>(Assets::getInstance().getAnimation("down")).animation;
		}

		animation.play();
	}
	if (pInput.left) {
		pv.x -= 1;

		auto& animation = playerPtr->getComponent<CAnimation>().animation;

		if (animation.getName() != "left")
		{
			animation = playerPtr->addComponent<CAnimation>(Assets::getInstance().getAnimation("left")).animation;
		}

		animation.play();
	}
	if (pInput.right) {
		pv.x += 1;

		auto& animation = playerPtr->getComponent<CAnimation>().animation;

		if (animation.getName() != "right")
		{
			animation = playerPtr->addComponent<CAnimation>(Assets::getInstance().getAnimation("right")).animation;
		}


		animation.play();
	}

	/*
	else {
		// Optionally, reset the sprite to the default when the left arrow key is not pressed
		auto& sprite = playerPtr->getComponent<CSprite>().sprite;
		auto [txtName, txtRect] = Assets::getInstance().getSprt("EagleStr");
		sprite.setTexture(Assets::getInstance().getTexture(txtName));
		sprite.setTextureRect(txtRect);
		centerOrigin(sprite);
	}

	if (pInput.up) pv.y -= 1;
	if (pInput.down) pv.y += 1;
	*/

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

void Scene_Bomb::sRender() {
	//m_game->window().clear(sf::Color(100, 100, 255));
	m_game->window().setView(m_worldView);

	for (auto& e : m_entityManager.getEntities())
	{
		if (e->getTag() == "player") continue;	// pula o player.

		renderEntity(e);	// desenhando os tiles (cenário) e as bombas.
	}

	// desenho os players no final e em cima de tudo.
	for (auto& e : m_entityManager.getEntities("player"))
	{
		renderEntity(e);
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

		// Player1 control
		else if (action.name() == "LEFT") { m_player1->getComponent<CInput>().left = true; }
		else if (action.name() == "RIGHT") { m_player1->getComponent<CInput>().right = true; }
		else if (action.name() == "UP") { m_player1->getComponent<CInput>().up = true; }
		else if (action.name() == "DOWN") { m_player1->getComponent<CInput>().down = true; }

		else if (action.name() == "BOMB") { dropBomb(m_player1); }

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


void Scene_Bomb::spawnPlayer(sPtrEntt& playerPtr, sf::Vector2f pos) {
	playerPtr = m_entityManager.addEntity("player");
	playerPtr->addComponent<CTransform>(pos);
	playerPtr->addComponent<CBoundingBox>(sf::Vector2f(16.0f, 16.0f), sf::Vector2f(0.0f, 8.0f));
	playerPtr->addComponent<CInput>();

	playerPtr->addComponent<CAnimation>(Assets::getInstance().getAnimation("down"));
}

void Scene_Bomb::dropBomb(sPtrEntt& player)
{
	auto& pos = player->getComponent<CTransform>().pos;

	// Transformei pixel em grid
	int x = (int)(pos.x / 16);
	int y = (int)((pos.y + 8) / 16); // +8 para ajustar o origem na bound box (nos pés)

	// Tranformar grid em pixel
	sf::Vector2f np((x * 16) + 8, y * 16); // O +8 em x é para ajustar a posição da bomba pq a origem é no centro.

	//o player so pode soltar uma bomba por vez
	if (m_entityManager.getEntities("Bomb").size() < 1)
	{
		spawnBomb(player, np);
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
	bomb->addComponent<CBomb>(player);

	//de o play na animacao
	bomb->getComponent<CAnimation>().animation.play();
}

bool Scene_Bomb::isColliding(sf::Vector2f pos) {
	// Verificar se a explosão colide com os tijolos destructable, tijolos não destructable ou paredes
	for (auto& e : m_entityManager.getEntities("Tile")) {
		if (e->hasComponent<CBoundingBox>()) {
			auto& tile = e->getComponent<CTransform>().pos;
			if (pos == tile) {
				auto tileType = e->getComponent<CTile>().type;
				//se colidir com um tijolo destructable, o tijolo é destruido
				if (tileType == TileType::Destructable || tileType == TileType::None) {
					return true;
				}
			}
		}
	}
	return false;
}

void Scene_Bomb::destroyDestructableTile(sf::Vector2f pos) {
	for (auto& e : m_entityManager.getEntities("Tile")) {
		if (e->hasComponent<CBoundingBox>()) {
			auto& tile = e->getComponent<CTransform>().pos;
			if (pos == tile) {
				auto tileType = e->getComponent<CTile>().type;
				//se colidir com um tijolo destructable, o tijolo é destruido
				if (tileType == TileType::Destructable) {
					e->removeComponent<CSprite>();
					e->addComponent<CAnimation>(Assets::getInstance().getAnimation("brick")).animation.play();

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

void Scene_Bomb::spawnExplosion(sf::Vector2f pos, int size)
{
	//FIRE MIDDLE (fireM)
	spawnFire(pos, "fireM");

	sf::Vector2f nPos = pos;
	//FIRE LEFT
	for (int i = 0; i < size; i++)
	{
		nPos.x -= 16;
		if (isColliding(nPos))
		{
			destroyDestructableTile(nPos);
			break;
		}
		if (i == size - 1)
			spawnFire(nPos, "fireL"); //FireL
		else
			spawnFire(nPos, "fireW"); //FireW
	}

	//FIRE RIGHT
	nPos = pos; //centralizo a posicao antes de poder ir para a direita
	for (int i = 0; i < size; i++)
	{
		nPos.x += 16;
		if (isColliding(nPos))
		{
			destroyDestructableTile(nPos);
			break;
		}
		if (i == size - 1)
			spawnFire(nPos, "fireR"); // FireR
		else
			spawnFire(nPos, "fireW"); // FireW
	}

	//FIRE UP
	nPos = pos; //centralizo a posicao antes de poder ir para cima
	for (int i = 0; i < size; i++)
	{
		nPos.y -= 16;
		if (isColliding(nPos))
		{
			destroyDestructableTile(nPos);
			break;
		}
		if (i == size - 1)
			spawnFire(nPos, "fireU"); // FireU
		else
			spawnFire(nPos, "fireH"); // FireH

	}

	//FIRE DOWN
	nPos = pos; //centralizo a posicao antes de poder ir para baixo
	for (int i = 0; i < size; i++)
	{
		nPos.y += 16;
		if (isColliding(nPos))
		{
			destroyDestructableTile(nPos);
			break;
		}
		if (i == size - 1)
			spawnFire(nPos, "fireD"); // FireD
		else
			spawnFire(nPos, "fireH"); // FireH
	}
}

void Scene_Bomb::spawnFire(sf::Vector2f pos, const std::string& animation)
{
	auto e = m_entityManager.addEntity("Fire");
	auto& anim = e->addComponent<CAnimation>(Assets::getInstance().getAnimation(animation));
	e->addComponent<CTransform>(pos);
	e->addComponent<CBoundingBox>(sf::Vector2f(16, 16));

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


void Scene_Bomb::sCollisions(sPtrEntt& playerPtr) {
	adjustPlayerPosition();

	//PLAYER AND TILE COLLISION
	for (auto& tile : m_entityManager.getEntities("Tile"))
	{
		if (tile->hasComponent<CBoundingBox>()) // Se o tile collide (se tem o yes ou no)
		{
			auto overlap = Physics::getOverlap(playerPtr, tile);
			auto prevOverlap = Physics::getPreviousOverlap(playerPtr, tile);

			if (overlap.x > 0 && overlap.y > 0) // se overlpar em x e y for maior que zero, ele entrou no tile
			{

				if (prevOverlap.x > 0) // Se entrar de cima ou para baixo vai ser nesse if
				{
					auto& pos = playerPtr->getComponent<CTransform>().pos;

					if (overlap.x < 7.5f) // 7.5 relacionado ao tamanho do cubo
					{
						if (pos.x > tile->getComponent<CTransform>().pos.x) // Entrou da direita
							if (overlap.x - 0.5f < 0)
							{
								pos.x += overlap.x;
							}
							else
							{
								pos.x += 0.5f;
							}
						//pos.x += overlap.x - 2.0f < 0 ? overlap.x : 2.0f;
						else // entrou da esquerda
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

					if (pos.y > tile->getComponent<CTransform>().pos.y) // entrou no bloco por baixo
						pos.y += overlap.y;
					else // entrou no bloco por cima
						pos.y -= overlap.y;
				}
				else // Se entrar da direita ou esquerda vai ser nesse else
				{
					auto& pos = playerPtr->getComponent<CTransform>().pos;

					if (overlap.y < 7.5f) // 7.5 relacionado ao tamanho do cubo
					{
						if (pos.y > tile->getComponent<CTransform>().pos.y) // entrou no bloco por baixo
							if (overlap.y - 0.5f < 0)
							{
								pos.y += overlap.y;
							}
							else
							{
								pos.y += 0.5f;
							}
						//pos.y += overlap.y - 0.5f < 0 ? overlap.y : 0.5f;
						else // entrou no bloco por cima
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
					else // entrou no bloco pela esquerda
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

		// Verificar se o jogador estava anteriormente sobre a bomba e agora não está mais
		//bool wasOnBomb = prevOverlap.y > 0 && overlap.y <= 0;

		// Verifica se o jogador está em cima da bomba primeiro
		auto& isOnBomb = bomb->getComponent<CBomb>().isOnBomb;
		if (isOnBomb)
		{
			auto& bombPlayer = bomb->getComponent<CBomb>().player; // BombPlayer é o player que soltou essa bomba.
			auto overlapBP = Physics::getOverlap(bombPlayer, bomb); // Overlap do Bomb Player.

			// Ele está, verificamos quando ele sair trocamos o isOnBomb para false.
			if (overlapBP.x < 0 || overlapBP.y < 0)
			{
				isOnBomb = false;
			}
		}
		else
		{
			// Ele não está mais na bomba;
			// Verificar se houve colisão e se o jogador estava anteriormente sobre a bomba
			if (overlap.x > 0 && overlap.y > 0 /* && wasOnBomb */) {
				// Ajustar a posição do jogador para evitar a colisão
				if (prevOverlap.x > 0)
				{
					// Colisão mais forte no eixo Y, mova o jogador verticalmente
					auto& pos = playerPtr->getComponent<CTransform>().pos;

					if (pos.y > bomb->getComponent<CTransform>().pos.y) // entrou no bloco por baixo
						pos.y += overlap.y;
					else // entrou no bloco por cima
						pos.y -= overlap.y;
				}
				else
				{
					// Colisão mais forte no eixo X, mova o jogador horizontalmente
					auto& pos = playerPtr->getComponent<CTransform>().pos;

					if (pos.x > bomb->getComponent<CTransform>().pos.x) // entrou no bloco pela direita
						pos.x += overlap.x;
					else // entrou no bloco pela esquerda
						pos.x -= overlap.x;
				}
			}
		}
	}

	// PLAYER AND POWERUP
	// TODO
	for(auto& powerUp : m_entityManager.getEntities("PowerUp"))
	{
		auto overlap = Physics::getOverlap(playerPtr, powerUp);
		auto prevOverlap = Physics::getPreviousOverlap(playerPtr, powerUp);

		if (overlap.x > 0 && overlap.y > 0)
		{
			auto& powerUpType = powerUp->getComponent<CAnimation>().animation.getName();
			if (powerUpType == "powerUp1")
			{
				m_fire++;
			}
			else if (powerUpType == "powerUp2")
			{
				m_bomb++;
			}
			else if (powerUpType == "powerUp3")
			{
				m_speed++;
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
	// TODO


	// FIRE AND BRICK COLLISION
	/*
	for (auto& explosion : m_entityManager.getEntities("Fire")) {
		for (auto& tile : m_entityManager.getEntities("Tile")) {
			if (tile->hasComponent<CBoundingBox>()) {
				auto overlapExplosionTile = Physics::getOverlap(explosion, tile);
				auto prevOverlapExplosionTile = Physics::getPreviousOverlap(explosion, tile);

				// Lógica de colisão entre explosões (fire) e tijolos (bricks)
				if (overlapExplosionTile.x > 0 && overlapExplosionTile.y > 0)
				{
					if (tile->getComponent<CTile>().type == TileType::Destructable)
					{
						tile->removeComponent<CSprite>(); // Destrua o tijolo
						auto& animation = tile->addComponent<CAnimation>(Assets::getInstance().getAnimation("brick")).animation;
						animation.play();
					}
				}
			}
		}
	}
	*/
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
			e->destroy();
			spawnExplosion(e->getComponent<CTransform>().pos, e->getComponent<CBomb>().explosionSize);
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