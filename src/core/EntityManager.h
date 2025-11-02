//
// Created by helpe on 21.10.2025.
//
#pragma once
#include <memory>
#include <vector>

#include "Entity.h"


class Entity;

class EntityManager {
private:

  std::vector<std::unique_ptr<Entity>> entities_;

  std::vector<EntityID> entities_to_destroy_;

public:

  EntityManager() = default;
  ~EntityManager() { clear(); }

  Entity* create_entity();

  void destroy_entity(const Entity* entity);
  void destroy_entity(EntityID id);

  Entity* get_entity(EntityID id) const;

  const std::vector<std::unique_ptr<Entity>>& get_entities() const { return entities_; }

  template <typename... Components>
  std::vector<Entity*> get_entities_with() const {
    std::vector<Entity*> result;
    for (const auto& entity : entities_) {
      if ((entity->has_component<Components>() && ...)) {
        result.push_back(entity.get());
      }
    }
    return result;
  }

  void cleanup_destroyed_entities();

  void clear();

  size_t get_entity_count() const { return entities_.size(); }
};
