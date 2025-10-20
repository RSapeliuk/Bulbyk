#include "TransformSystem.h"

#include <algorithm>

#include "utils.h"


void TransformSystem::register_entity(Entity *entity) {
  if (!is_valid_entity(entity)) {
    TRACELOG(LOG_WARNING, "Entity %d doesn't have required components (Transform + Sprite)!", entity->get_id());
    return;
  }

  if (const auto it = std::ranges::find(entities_, entity); it != entities_.end()) {
    TRACELOG(LOG_WARNING, "Entity %d already registered in TransformSystem!", entity->get_id());
    return;
  }

  entities_.push_back(entity);
  TRACELOG(LOG_INFO, "Entity %d registered in TransformSystem", entity->get_id());
}

void TransformSystem::unregister_entity(const Entity *entity) {
  if (const auto it = std::ranges::find(entities_, entity); it != entities_.end()) {
    entities_.erase(it);
    TRACELOG(LOG_INFO, "Entity %d unregistered from TransformSystem", entity->get_id());
  }
}

void TransformSystem::update(const float delta_time) {
  for (const Entity *entity : entities_) {
    if (!is_valid_entity(entity)) {
      continue;
    }

    Components::Transform *transform = entity->get_component<Components::Transform>();
    transform->position.x += transform->velocity.x * delta_time;
    transform->position.y += transform->velocity.y * delta_time;


    if (transform->velocity.x != 0.0f || transform->velocity.y != 0.0f) {
      TRACELOG(LOG_INFO, "Entity %d moved to (%f, %f)", entity->get_id(), transform->position.x, transform->position.y);
    }
  }
}

void TransformSystem::set_velocity(const Entity *entity, const Vector2 velocity) {
  if (!is_valid_entity(entity)) {
    return;
  }

  Components::Transform *transform = entity->get_component<Components::Transform>();
  transform->velocity = velocity;
}

void TransformSystem::set_position(const Entity *entity, const Vector2 position) {
  if (!is_valid_entity(entity)) {
    return;
  }

  Components::Transform *transform = entity->get_component<Components::Transform>();
  transform->position = position;
}

void TransformSystem::move(const Entity *entity, const Vector2 offset) {
  if (!is_valid_entity(entity)) {
    return;
  }

  Components::Transform *transform = entity->get_component<Components::Transform>();
  transform->position.x += offset.x;
  transform->position.y += offset.y;
}

Vector2 TransformSystem::get_position(const Entity *entity) {
  if (!is_valid_entity(entity)) {
    return Vector2{0.0f, 0.0f};
  }

  const Components::Transform *transform = entity->get_component<Components::Transform>();
  return transform->position;
}

void TransformSystem::clamp_to_world_bounds(const Entity *entity, const Rectangle world_bounds) {
  if (!is_valid_entity(entity)) {
    return;
  }

  Components::Transform *transform = entity->get_component<Components::Transform>();

  if (transform->position.x < world_bounds.x) {
    transform->position.x = world_bounds.x;
    transform->velocity.x = 0.0f;
  }
  if (transform->position.y < world_bounds.y) {
    transform->position.y = world_bounds.y;
    transform->velocity.y = 0.0f;
  }
  if (transform->position.x > world_bounds.x + world_bounds.width) {
    transform->position.x = world_bounds.x + world_bounds.width;
    transform->velocity.x = 0.0f;
  }
  if (transform->position.y > world_bounds.y + world_bounds.height) {
    transform->position.y = world_bounds.y + world_bounds.height;
    transform->velocity.y = 0.0f;
  }
}

void TransformSystem::clear_entities() {
  entities_.clear();
  TRACELOG(LOG_INFO, "TransformSystem cleared all entities");
}

bool TransformSystem::is_valid_entity(const Entity *entity) {
  return entity && entity->has_component<Components::Transform>();
}



