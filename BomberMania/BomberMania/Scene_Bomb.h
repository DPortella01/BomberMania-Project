//
// Created by David Burchill on 2023-09-27.
//

#ifndef BREAKOUT_SCENE_BREAKOUT_H
#define BREAKOUT_SCENE_BREAKOUT_H

#include <SFML/System/Time.hpp>
#include <SFML/Graphics.hpp>
#include "EntityManager.h"
#include "Entity.h"
#include "Scene.h"
#include "GameEngine.h"


struct LevelConfig {
	float       scrollSpeed{ 100.f };
	float       playerSpeed{ 200.f };
};


class Scene_Bomb : public Scene {
private:
	sPtrEntt        m_player1{ nullptr };
	sPtrEntt        m_player2{ nullptr };
	sPtrEntt        m_player3{ nullptr };
	sPtrEntt        m_player4{ nullptr };

	sf::View        m_worldView;
	sf::FloatRect   m_worldBounds;

	LevelConfig     m_config;

	bool			m_drawTextures{ true };
	bool			m_drawAABB{ false };
	bool			m_drawGrid{ false };

	// PowerUp list
	int				m_fire = 5;
	int				m_bomb = 5;
	int				m_speed = 3;

	// systems
	void            sMovement(sf::Time dt);
	void            sCollisions(sPtrEntt& playerPtr);
	void            sUpdate(sf::Time dt);
	void            sAnimation(sf::Time dt);

	void	        onEnd() override;

	// helper functions
	void            playerMovement(sPtrEntt& playerPtr);
	void            adjustPlayerPosition();

	void	        registerActions();
	void            spawnPlayer(sPtrEntt& playerPtr, sf::Vector2f pos);

	void            init(const std::string& path);
	void            loadLevel(const std::string& path);
	sf::FloatRect   getViewBounds();

	void			dropBomb(sPtrEntt& player);
	void			spawnBomb(sPtrEntt& player, sf::Vector2f pos);
	bool			isColliding(sf::Vector2f pos);
	bool			isDestructable(sf::Vector2f pos);
	void			destroyDestructableTile(sf::Vector2f pos);
	void			spawnExplosion(sf::Vector2f pos, int size);
	void			spawnFire(sf::Vector2f pos, const std::string& animation);
	void 			spawnPowerUp(sf::Vector2f pos, const std::string& type);
	void			spawnBrick(sf::Vector2f pos);

	void			spawnRandomBricks();

	void			sBombUpdate(sf::Time dt);

	void			renderEntity(std::shared_ptr<Entity>& e);


public:

	Scene_Bomb(GameEngine* gameEngine, const std::string& levelPath);

	void		  update(sf::Time dt) override;
	void		  sDoAction(const Command& command) override;
	void		  sRender() override;

};


#endif //BREAKOUT_SCENE_BREAKOUT_H