#pragma once
#include "EntityManager.h"


class Game
{
private:
	sf::RenderWindow m_window;
	EntityManager m_entities;
	sf::Font m_font;
	sf::Text m_text;
	int m_score = 0;
	int m_currentFrame = 0;
	int m_lastEnemySpawnTime = 0;
	int m_lastShieldTime = 0;
	bool m_paused = false;
	bool m_running = true;

	std::shared_ptr<Entity>  m_player;

	void init();
	void setPaused(bool);

	void sMovement();
	void sUserInput();
	void sLifespan();
	void sRender();
	void sEnemySpawner();
	void sCollision();
	void sScore();

	void spawnPlayer();
	void spawnEnemy();
	void spawnSmallEnemies(std::shared_ptr<Entity>);
	void spawnBullet(std::shared_ptr<Entity>, const Vec2&);
	void spawnShield(std::shared_ptr<Entity>);
	void defeat(const std::string&);
	void controlMovement(std::shared_ptr<Entity>);
	void shield(const std::string&);

public:
	Game(const std::string&);

	void run();
};