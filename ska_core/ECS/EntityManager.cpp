#include "EntityManager.h"
#include "../Exceptions/IllegalStateException.h"
#include "../Utils/StringUtils.h"
#include "../Data/Events/GameEvent.h"
#include "../Data/Events/ECSEvent.h"

void ska::EntityManager::commonRemoveComponent(EntityId entity, ComponentSerializer& components) {
	auto removedComponentMask = components.remove(entity);
    m_componentMask[entity][removedComponentMask] = false;

    m_alteredEntities.insert(entity);
}

void ska::EntityManager::refresh() {
    for(const auto& entity : m_alteredEntities) {
		notifyObservers(COMPONENT_ALTER, m_componentMask[entity], entity);
    }
    m_alteredEntities.clear();
}


void ska::EntityManager::commonAddComponent(EntityId entity, const unsigned int componentMask) {
    m_componentMask[entity][componentMask] = true;

    m_alteredEntities.insert(entity);
}


const std::string ska::EntityManager::serializeComponent(const EntityId entityId, const std::string& component, const std::string& field) const {
    if (NAME_MAPPED_COMPONENT.find(component) != NAME_MAPPED_COMPONENT.end()) {
        return NAME_MAPPED_COMPONENT.at(component)->getComponentField(entityId, field);
    }
    return "";
}

void ska::EntityManager::removeComponent(const EntityId entity, const std::string& component) {
    if (NAME_MAPPED_COMPONENT.find(component) != NAME_MAPPED_COMPONENT.end()) {
	    auto components = NAME_MAPPED_COMPONENT.at(component);
        commonRemoveComponent(entity, *components);
    }
}

void ska::EntityManager::addComponent(const EntityId entity, const std::string& component) {
    if (NAME_MAPPED_COMPONENT.find(component) != NAME_MAPPED_COMPONENT.end()) {
        ComponentSerializer * components = NAME_MAPPED_COMPONENT.at(component);
        commonAddComponent(entity, components->addEmpty(entity));
    }
}

ska::EntityId ska::EntityManager::createEntityNoThrow() {
	EntityId newId;

	if(!checkEntitiesNumber()) {
		m_entities.erase(std::begin(m_entities));
	}

	if (m_deletedEntities.empty()) {
		newId = static_cast<EntityId>(m_entities.size());
	} else {
		newId = m_deletedEntities[m_deletedEntities.size() - 1];
		m_deletedEntities.pop_back();		
	}
	m_entities.insert(newId);

	/* Reset all components */
	m_componentMask[newId] &= 0;

	m_alteredEntities.insert(newId);

	return newId;
}

bool ska::EntityManager::checkEntitiesNumber() const {
	return static_cast<int>(m_entities.size() - m_deletedEntities.size()) < SKA_ECS_MAX_ENTITIES;
}

ska::EntityId ska::EntityManager::createEntity() {
    if (!checkEntitiesNumber()) {
        throw IllegalStateException("Too many entities are currently in use. Unable to create a new one. "
            "Increase SKA_ECS_MAX_ENTITIES at compile time to avoid the problem.");
    }

	return createEntityNoThrow();
}

void ska::EntityManager::removeEntity(EntityId entity) {
	ECSEvent ecsEvent(ENTITIES_REMOVED);
	innerRemoveEntity(entity, ecsEvent);
	m_ged.ska::Observable<ECSEvent>::notifyObservers(ecsEvent);
	
	/* Reset all components */
	m_componentMask[entity] &= 0;
}

void ska::EntityManager::innerRemoveEntity(EntityId entity, ECSEvent& ecsEvent) {
	if (m_entities.find(entity) == m_entities.end() || m_entities.count(entity) <= 0) {
		auto startMessage = ("Unable to delete entity #" + StringUtils::intToStr(static_cast<int>(entity)));
		SKA_LOG_ERROR(startMessage + " : this entity doesn't exist or is already deleted");
		return;
	}

	m_entities.erase(entity);

	m_deletedEntities.push_back(entity);

	m_alteredEntities.insert(entity);
	
	ecsEvent.entities.emplace(entity);
}

void ska::EntityManager::removeEntities(const std::unordered_set<EntityId>& exceptions) {
	ECSEvent ecsEvent(ENTITIES_REMOVED);
	auto entities = m_entities;
    for (const auto& entity : entities) {
        if (exceptions.find(entity) == exceptions.end()) {
            innerRemoveEntity(entity, ecsEvent);
        }
    }
	m_ged.ska::Observable<ECSEvent>::notifyObservers(ecsEvent);

	for(auto& entity : ecsEvent.entities) {
		/* Reset all components */
		m_componentMask[entity] &= 0;
	}
}

void ska::EntityManager::refreshEntity(EntityId entity) {
	notifyObservers(COMPONENT_ALTER, m_componentMask[entity], entity);
}

void ska::EntityManager::refreshEntities() {
    for (const auto& entity : m_entities) {
		notifyObservers(COMPONENT_ALTER, m_componentMask[entity], entity);
    }
}
