//
// Created by helpe on 21.10.2025.
//

#include "EntityManager.h"

#include <algorithm>
#include <utils.h>



Entity *EntityManager::create_entity() {
  auto entity = std::make_unique<Entity>();
  Entity *entity_ptr = entity.get();

  entities_.push_back(std::move(entity));

  TRACELOG(LOG_INFO, "Entity %d created", entity_ptr->get_id());
  return entity_ptr;
}

void EntityManager::destroy_entity(EntityID id) {
  entities_to_destroy_.push_back(id);
  TRACELOG(LOG_INFO, "Entity %d marked for destroy", id);
}

void EntityManager::destroy_entity(const Entity *entity) {
  if (entity) {
    destroy_entity(entity->get_id());
  }
}

Entity *EntityManager::get_entity(EntityID id) const {
  for (const auto &entity : entities_) {
    if (entity->get_id() == id) {
      return entity.get();
    }
  }
  return nullptr;
}

void EntityManager::cleanup_destroyed_entities() {
  if (entities_to_destroy_.empty()) {
    return;
  }

  TRACELOG(LOG_INFO, "Cleaning up %s destroyed entities", entities_to_destroy_.size());
  for (const auto &id : entities_to_destroy_) {
    auto it = std::ranges::remove_if(entities_, [id](const std::unique_ptr<Entity> &entity) {
      return entity->get_id() == id;
    }).begin();

    if (it != entities_.end()) {
      TRACELOG(LOG_INFO, "Entity %d destroyed", id);
      entities_.erase(it);
    }
  }
  entities_to_destroy_.clear();
}

void EntityManager::clear() {
  TRACELOG(LOG_INFO, "Clearing all entities");
  entities_.clear();
  entities_to_destroy_.clear();
}



