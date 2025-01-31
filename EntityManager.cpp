#include "EntityManager.h"

#include <iostream>

EntityManager::EntityManager()
{

}

const EntityVec& EntityManager::getEntities()
{
	return m_entities;
}

const EntityVec& EntityManager::getEntities(const std::string& tag)
{
	return m_entityMap[tag];
}

void EntityManager::update()
{
	for (auto& entity : m_entitiesToAdd)
	{
		m_entities.push_back(entity);
		m_entityMap[entity->tag()].push_back(entity);
	}

    m_entities.erase
    (
        std::remove_if
        (m_entities.begin(), m_entities.end(),
            [this](const auto& entity)
            {
                if (!entity->isActive())
                {
                    m_entityMap[entity->tag()].erase
                    (
                        std::remove
                        (
                            m_entityMap[entity->tag()].begin(),
                            m_entityMap[entity->tag()].end(),
                            entity
                        ),
                        m_entityMap[entity->tag()].end()
                    );
                    return true;
                }
                return false;
            }
        ),
        m_entities.end()
    );

	m_entitiesToAdd.clear();
}

std::shared_ptr<Entity> EntityManager::addEntity(const std::string& tag)
{
    std::shared_ptr<Entity> entity = std::shared_ptr<Entity>(new Entity(m_totalEntities++, tag));
	m_entitiesToAdd.push_back(entity);
	return entity;
}