#include "Game.h"

#include <string>
#include <iostream>
#include <cmath>
#include <ctime>

Game::Game(const std::string& statement)
{
	std::cout << statement << std::endl;
	init();
}

void Game::init()
{
	srand(time(NULL));
	m_window.create(sf::VideoMode::getDesktopMode(), "Assignment 2", sf::Style::None);
	m_window.setFramerateLimit(60);

	sScore();
	spawnPlayer();
}

void Game::run()
{
	while (m_running)
	{
		m_entities.update();

		if (!m_paused)
		{
			sEnemySpawner();
			sMovement();
			sCollision();
			sLifespan();
		}
		
		sUserInput();
		m_text.setString("Score: " + std::to_string(m_score));
		sRender();

		m_currentFrame++;
	}
}

void Game::sScore()
{
	if (!m_font.loadFromFile("Algerian.ttf"))
	{
		std::cout << "Couldn't load font" << std::endl;
	}
	m_text.setFont(m_font);
	m_text.setString("SCORE: 0");
	m_text.setCharacterSize(30);
	m_text.setFillColor(sf::Color::White);
	m_text.setPosition(0.0f, 0.0f);
	m_text.setStyle(sf::Text::Bold);
}

void Game::setPaused(bool paused)
{
	if (!paused)
	{
		m_paused = true;
	}
	else
	{
		m_paused = false;
	}
}

void Game::spawnPlayer()
{
	auto entity = m_entities.addEntity("player");

	float mx = m_window.getSize().x / 2.0f;
	float my = m_window.getSize().y / 2.0f;

	entity->cTransform = std::make_shared<CTransform>(Vec2(mx, my), Vec2(0.0f, 0.0f), 0, 0.0f, 0);

	entity->cShape = std::make_shared<CShape>(32.0f, 8, sf::Color(10, 10, 10), sf::Color(255, 0, 0), 4.0f);

	entity->cInput = std::make_shared<CInput>();

	entity->cLifespan = std::make_shared<CLifespan>();

	entity->cCollision = std::make_shared<CCollision>(entity->cShape->circle.getRadius()+5.0f);

	m_player = entity;
}

void Game::spawnEnemy()
{
	auto entity = m_entities.addEntity("enemy");

	int points = 3 + rand() % 5;

	int r = 0 + rand() % 255;
	int g = 0 + rand() % 255;
	int b = 0 + rand() % 255;

	entity->cShape = std::make_shared<CShape>(16.0f, points, sf::Color(r, g, b), sf::Color(255, 255, 255), 4.0f);

	entity->cCollision = std::make_shared<CCollision>(entity->cShape->circle.getRadius());

	float ex = entity->cCollision->radius + rand() % static_cast<int>(m_window.getSize().x - 2 * entity->cCollision->radius);
	float ey = entity->cCollision->radius + m_text.getCharacterSize() + rand() % static_cast<int>(m_window.getSize().y - 2 * (entity->cCollision->radius + m_text.getCharacterSize()));



	int theta = 0 + rand() % 360;
	int speed = 2 + rand() % 4;

	entity->cTransform = std::make_shared<CTransform>(Vec2(ex, ey), Vec2(speed * cos(theta * (3.14 / 180.0f)), speed * sin(theta * (3.14 / 180.0f))), theta, 0.0f, speed);

	entity->cLifespan = std::make_shared<CLifespan>();

	entity->cScore = std::make_shared<CScore>((points+speed)/2*10);

	m_lastEnemySpawnTime = m_currentFrame;
}

void Game::spawnSmallEnemies(std::shared_ptr<Entity> entity)
{
	std::shared_ptr<Entity> smallEnemy;
	float inc = 360.0f / entity->cShape->circle.getPointCount();
	int speed = 1;
	
	for (float theta = 0.0f; theta < 360.0f; theta+=inc)
	{
		smallEnemy = m_entities.addEntity("small enemy");

		Vec2 posSmallEnemy = entity->cTransform->pos;

		smallEnemy->cTransform = std::make_shared<CTransform>(posSmallEnemy, Vec2(speed*cos(theta * (3.14 / 180.0f)), speed*sin(theta * (3.14 / 180.0f))), theta, 0.0f, speed);

		smallEnemy->cShape = std::make_shared<CShape>(10.0f, entity->cShape->circle.getPointCount(), entity->cShape->circle.getFillColor(), entity->cShape->circle.getOutlineColor(), 4.0f);

		smallEnemy->cLifespan = std::make_shared<CLifespan>(50);

		smallEnemy->cCollision = std::make_shared<CCollision>(smallEnemy->cShape->circle.getRadius());
	}
}

void Game::spawnBullet(std::shared_ptr<Entity> entity, const Vec2& target)
{
	auto bullet = m_entities.addEntity("bullet");

	float theta = atan2f((target - entity->cTransform->pos).y, (target - entity->cTransform->pos).x);
	int speed = 5;

	bullet->cTransform = std::make_shared<CTransform>(entity->cTransform->pos, Vec2(speed*cos(theta), speed*sin(theta)), theta, 0.0f, speed);

	bullet->cShape = std::make_shared<CShape>(10.0f, 32, sf::Color(255, 255, 255), sf::Color(255, 0, 0), 2.0f);

	bullet->cLifespan = std::make_shared<CLifespan>(100);

	bullet->cCollision = std::make_shared<CCollision>(bullet->cShape->circle.getRadius());
}

void Game::sLifespan()
{
	for (auto& entity : m_entities.getEntities())
	{
		if (entity->cLifespan->total != -1)
		{
			if (entity->cLifespan->remaining > 0)
			{
				entity->cLifespan->remaining--;

				float initialLifespan = entity->cLifespan->total;
				float alpha = (entity->cLifespan->remaining / initialLifespan) * 255;

				alpha = std::max(0.0f, std::min(255.0f, alpha));

				sf::Color currentColor = entity->cShape->circle.getFillColor();
				sf::Color currentBorder = entity->cShape->circle.getOutlineColor();

				currentColor.a = static_cast<sf::Uint8>(alpha);
				entity->cShape->circle.setFillColor(currentColor);

				currentBorder.a = static_cast<sf::Uint8>(alpha);
				entity->cShape->circle.setOutlineColor(currentBorder);

				if (entity->cLifespan->remaining == 0)
				{
					entity->destroy();
				}
			}
		}
	}
}

void Game::spawnShield(std::shared_ptr<Entity> player)
{
	auto entity = m_entities.addEntity("shield");

	Vec2 position = player->cTransform->pos;

	entity->cTransform = std::make_shared<CTransform>(position, Vec2(0.0f, 0.0f), 0, 0.0f, 0);

	entity->cShape = std::make_shared<CShape>(64.0f, 8, sf::Color(0, 0, 255), sf::Color(255, 255, 255), 4.0f);

	entity->cCollision = std::make_shared<CCollision>(entity->cShape->circle.getRadius()+5.0f);

	entity->cLifespan = std::make_shared<CLifespan>(250);

	m_lastShieldTime = m_currentFrame;
}

void Game::controlMovement(std::shared_ptr<Entity> entity)
{
	if (entity->cShape->circle.getPosition().y - entity->cCollision->radius <= m_text.getCharacterSize())
	{
		m_player->cInput->up = false;
	}
	if (entity->cShape->circle.getPosition().y + entity->cCollision->radius >= m_window.getSize().y)
	{
		m_player->cInput->down = false;
	}
	if (entity->cShape->circle.getPosition().x - entity->cCollision->radius <= 0)
	{
		m_player->cInput->left = false;
	}
	if (entity->cShape->circle.getPosition().x + entity->cCollision->radius >= m_window.getSize().x)
	{
		m_player->cInput->right = false;
	}
}

void Game::sMovement()
{
	m_player->cTransform->velocity = { 0.0f, 0.0f };

	if (!m_entities.getEntities("shield").empty())
	{
		auto& shield = m_entities.getEntities("shield").front();
		shield->cTransform->velocity = {0.0f, 0.0f};
		controlMovement(shield);
	}

	controlMovement(m_player);
	if (m_player->cInput->up)
	{
		if (!m_entities.getEntities("shield").empty())
		{
			auto& shield = m_entities.getEntities("shield").front();
			shield->cTransform->velocity.y = -5.0f;
		}
		m_player->cTransform->velocity.y = -5.0f;
	}
	if (m_player->cInput->left)
	{
		if (!m_entities.getEntities("shield").empty())
		{
			auto& shield = m_entities.getEntities("shield").front();
			shield->cTransform->velocity.x = -5.0f;
		}
		m_player->cTransform->velocity.x = -5.0f;
	}
	if (m_player->cInput->down)
	{
		if (!m_entities.getEntities("shield").empty())
		{
			auto& shield = m_entities.getEntities("shield").front();
			shield->cTransform->velocity.y = 5.0f;
		}
		m_player->cTransform->velocity.y = 5.0f;
	}
	if (m_player->cInput->right)
	{
		if (!m_entities.getEntities("shield").empty())
		{
			auto& shield = m_entities.getEntities("shield").front();
			shield->cTransform->velocity.x = 5.0f;
		}
		m_player->cTransform->velocity.x = 5.0f;
	}

	const float PI = 3.14159265358979f;
	for (auto& entity : m_entities.getEntities("enemy")) {
		auto& pos = entity->cShape->circle.getPosition();
		auto& radius = entity->cCollision->radius;
		auto& angle = entity->cTransform->angle;
		auto& speed = entity->cTransform->speed;

		if (pos.y - radius <= m_text.getCharacterSize() || pos.y + radius >= m_window.getSize().y) {
			std::cout << "Boundary touched" << std::endl;
			angle = -angle;
			entity->cTransform->velocity = Vec2(speed * cos(angle * (PI / 180.0f)), speed * sin(angle * (PI / 180.0f)));
		}

		if (pos.x - radius <= 0 || pos.x + radius >= m_window.getSize().x) {
			std::cout << "Boundary touched" << std::endl;
			angle = 180 - angle;
			entity->cTransform->velocity = Vec2(speed * cos(angle * (PI / 180.0f)), speed * sin(angle * (PI / 180.0f)));
		}
	}

	for (auto& entity : m_entities.getEntities())
	{
		entity->cTransform->pos += entity->cTransform->velocity;
	}
}


void Game::sUserInput()
{
	sf::Event event;
	while (m_window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
		{
			m_running = false;
			std::cout << "Game Closed" << std::endl;
		}

		if (event.type == sf::Event::KeyPressed)
		{
			switch (event.key.code)
			{
			case sf::Keyboard::W:
				m_player->cInput->up = true;
				break;
			case sf::Keyboard::A:
				m_player->cInput->left = true;
				break;
			case sf::Keyboard::S:
				m_player->cInput->down = true;
				break;
			case sf::Keyboard::D:
				m_player->cInput->right = true;
				break;
			case sf::Keyboard::Escape:
				m_running = false;
				std::cout << "Game Closed" << std::endl;
				break;
			case sf::Keyboard::Space:
				setPaused(m_paused);
				break;
			}
		}

		if (event.type == sf::Event::KeyReleased)
		{
			switch (event.key.code)
			{
			case sf::Keyboard::W:
				m_player->cInput->up = false;
				break;
			case sf::Keyboard::A:
				m_player->cInput->left = false;
				break;
			case sf::Keyboard::S:
				m_player->cInput->down = false;
				break;
			case sf::Keyboard::D:
				m_player->cInput->right = false;
				break;
			}
		}

		if (event.type == sf::Event::MouseButtonPressed)
		{
			if (event.mouseButton.button == sf::Mouse::Left)
			{
				spawnBullet(m_player, Vec2(event.mouseButton.x, event.mouseButton.y));
			}

			if (event.mouseButton.button == sf::Mouse::Right)
			{
				if (m_currentFrame - m_lastShieldTime >= 500 && !m_paused)
				{
					std::cout << "Shield deployed!!!!!" << std::endl;
					spawnShield(m_player);
				}
				else
				{
					std::cout << "You cannot spawn the shield right now" << std::endl;
				}
			}
		}

		if (event.type == sf::Event::LostFocus)
		{
			m_paused = true;
			std::cout << "Lost focus. Game paused" << std::endl;
		}

		if (event.type == sf::Event::GainedFocus)
		{
			std::cout << "Gained focus" << std::endl;
		}
	}
}

void Game::sEnemySpawner()
{
	if (m_currentFrame - m_lastEnemySpawnTime >= 150)
	{
		spawnEnemy();
	}
}

void Game::defeat(const std::string& tag)
{
	for (auto& enemy : m_entities.getEntities(tag))
	{
		float distPlayer = Vec2(m_player->cShape->circle.getPosition().x, m_player->cShape->circle.getPosition().y).distsq(Vec2(enemy->cShape->circle.getPosition().x, enemy->cShape->circle.getPosition().y));
		float radiusSumPlayer = m_player->cCollision->radius + enemy->cCollision->radius;

		if (distPlayer <= radiusSumPlayer * radiusSumPlayer)
		{
			m_score = 0;
			m_player->destroy();
			spawnPlayer();
		}
	}
}

void Game::shield(const std::string& tag)
{
	auto& shield = m_entities.getEntities("shield").front();
	for (auto& enemy : m_entities.getEntities(tag))
	{
		float distShield = Vec2(shield->cShape->circle.getPosition().x, shield->cShape->circle.getPosition().y).distsq(Vec2(enemy->cShape->circle.getPosition().x, enemy->cShape->circle.getPosition().y));
		float radiusSumShield = shield->cCollision->radius + enemy->cCollision->radius;

		if (distShield <= radiusSumShield * radiusSumShield)
		{
			if (tag == "enemy")
			{
				m_score += enemy->cScore->score;
				spawnSmallEnemies(enemy);
			}
			enemy->destroy();
		}
	}
}

void Game::sCollision()
{
	defeat("enemy");
	defeat("small enemy");


	if (!m_entities.getEntities("shield").empty())
	{
		shield("enemy");
		shield("small enemy");
	}

	for (auto& bullet : m_entities.getEntities("bullet"))
	{
		for (auto& enemy : m_entities.getEntities("enemy"))
		{
			float distBullet = Vec2(bullet->cShape->circle.getPosition().x, bullet->cShape->circle.getPosition().y).distsq(Vec2(enemy->cShape->circle.getPosition().x, enemy->cShape->circle.getPosition().y));
			float radiusSumBullet = bullet->cCollision->radius + enemy->cCollision->radius;

			if (distBullet <= radiusSumBullet * radiusSumBullet)
			{
				m_score += enemy->cScore->score;
				spawnSmallEnemies(enemy);
				enemy->destroy();
				bullet->destroy();
			}
		}
	}

}


void Game::sRender()
{
	m_window.clear();
	for (auto& e : m_entities.getEntities())
	{
		e->cShape->circle.setPosition(e->cTransform->pos.x, e->cTransform->pos.y);
		e->cTransform->rotation += 1.0f;
		e->cShape->circle.setRotation(e->cTransform->rotation);
		m_window.draw(e->cShape->circle);
	}
	m_window.draw(m_text);
	m_window.display();
	
}