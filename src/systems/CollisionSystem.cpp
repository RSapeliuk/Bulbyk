//
// Created by helpe on 22.10.2025.
//

#include "CollisionSystem.h"

#include <cmath>

#include "utils.h"
#include "components/Transform.h"

using namespace Components;

void CollisionSystem::register_entity(Entity *entity) {
  if (!is_valid_entity(entity)) {
    TRACELOG(LOG_WARNING, "Entity %d doesn't have required components (Transform + Collider)!", entity->get_id());
    return;
  }

  if (const auto it = std::ranges::find(entities_, entity); it != entities_.end()) {
    TRACELOG(LOG_WARNING, "Entity %d already registered in CollisionSystem!", entity->get_id());
    return;
  }
  entities_.push_back(entity);
  TRACELOG(LOG_INFO, "Entity %d registered in CollisionSystem", entity->get_id());
}

void CollisionSystem::unregister_entity(const Entity *entity) {
  if (const auto it = std::ranges::find(entities_, entity); it != entities_.end()) {
    entities_.erase(it);
    TRACELOG(LOG_INFO, "Entity %d unregistered from CollisionSystem", entity->get_id());
  }
}

void CollisionSystem::update() {
  for (size_t i = 0; i < entities_.size(); ++i) {
    for (size_t j = i + 1; j < entities_.size(); ++j) {
      Entity *entity_a = entities_[i];
      Entity *entity_b = entities_[j];

      CollisionInfo info;
      if (check_collision(entity_a, entity_b, &info)) {
        resolve_collision(info);
        for (const auto &callback: collision_callbacks_) {
          callback(info);
        }
      }
    }
  }
}

bool CollisionSystem::check_collision(Entity *entity_a, Entity *entity_b, CollisionInfo *out_info) const {
  if (!is_valid_entity(entity_a) || !is_valid_entity(entity_b)) {
    return false;
  }

  auto *transform_a = entity_a->get_component<Components::Transform>();
  auto *transform_b = entity_b->get_component<Components::Transform>();
  auto *collider_a = entity_a->get_component<Collider>();
  auto *collider_b = entity_b->get_component<Collider>();

  if (!should_collide(collider_a, collider_b)) {
    return false;
  }

  bool collision = false;
  CollisionInfo temp_info;
  temp_info.entity_a = entity_a;
  temp_info.entity_b = entity_b;

  if (collider_a->type == ColliderType::CIRCLE && collider_b->type == ColliderType::CIRCLE) {
    collision = circle_vs_circle(transform_a->position, collider_a->radius, transform_b->position, collider_b->radius,
                                 &temp_info);
  } else if (collider_a->type == ColliderType::RECTANGLE && collider_b->type == ColliderType::RECTANGLE) {
    collision = rect_vs_rect(transform_a->position, collider_a->size, transform_b->position, collider_b->size,
                             &temp_info);
  } else {
    if (collider_a->type == ColliderType::CIRCLE) {
      collision = circle_vs_rect(transform_a->position, collider_a->radius, transform_b->position, collider_b->size,
                                 &temp_info);
    } else {
      collision = circle_vs_rect(transform_b->position, collider_b->radius, transform_a->position, collider_a->size,
                                 &temp_info);
    }
  }

  if (collision && out_info) {
    *out_info = temp_info;
  }

  return collision;
}

bool CollisionSystem::circle_vs_circle(Vector2 pos_a, float radius_a, Vector2 pos_b, float radius_b,
                                       CollisionInfo *out_info) const {
  float dx = pos_b.x - pos_a.x;
  float dy = pos_b.y - pos_a.y;
  float distance_sq = dx * dx + dy * dy;

  float combined_radius = radius_a + radius_b;
  float combined_radius_sq = combined_radius * combined_radius;

  if (distance_sq > combined_radius_sq) {
    return false;
  }

  if (out_info) {
    float distance = std::sqrt(distance_sq);

    if (distance > 0.0001f) {
      out_info->collision_normal = Vector2(dx / distance, dy / distance);
    } else {
      out_info->collision_normal = Vector2(1.0f, 0.0f);
    }

    out_info->penetration_depth = combined_radius - distance;
    out_info->collision_point = Vector2(pos_a.x + out_info->collision_normal.x * radius_a,
                                        pos_a.y + out_info->collision_normal.y * radius_a);
  }
  return true;
}

bool CollisionSystem::rect_vs_rect(Vector2 pos_a, Vector2 size_a, Vector2 pos_b, Vector2 size_b, CollisionInfo *out_info) const {

  Rectangle rect_a = {pos_a.x -size_a.x / 2.0f, pos_a.y - size_a.y / 2.0f, size_a.x, size_a.y};
  Rectangle rect_b = {pos_b.x - size_b.x / 2.0f, pos_b.y - size_b.y / 2.0f, size_b.x, size_b.y};

  bool collision = CheckCollisionRecs(rect_a, rect_b);

  if (collision && out_info) {
    out_info->collision_point = Vector2{(pos_a.x + pos_b.x)/2, (pos_a.y + pos_b.y)/2};

    float dx = pos_b.x - pos_a.x;
    float dy = pos_b.y - pos_a.y;
    float length = std::sqrt(dx * dx + dy * dy);

    if (length > 0.0001f) {
      out_info->collision_normal = Vector2(dx / length, dy / length);
    }

    out_info->penetration_depth = 1.f;
  }
  return collision;
}

bool CollisionSystem::circle_vs_rect(Vector2 circle_pos, float radius, Vector2 rect_pos, Vector2 rect_size, CollisionInfo *out_info) const {
  float half_width = rect_size.x / 2.0f;
  float half_height = rect_size.y / 2.0f;

  float closest_x = std::clamp(circle_pos.x, rect_pos.x - half_width, rect_pos.x + half_width);
  float closest_y = std::clamp(circle_pos.y, rect_pos.y - half_height, rect_pos.y + half_height);

  float dx = closest_x - circle_pos.x;
  float dy = closest_y - circle_pos.y;
  float distance_sq = dx * dx + dy * dy;

  if (distance_sq > (radius * radius)) {
    return false;
  }

  if (out_info) {
    float distance = std::sqrt(distance_sq);

    if (distance > 0.0001f) {
      out_info->collision_normal = Vector2(dx / distance, dy / distance);
    } else {
      out_info->collision_normal = Vector2(0.0f, -1.0f);
    }

    out_info->penetration_depth = radius - distance;
    out_info->collision_point = Vector2(closest_x, closest_y);
  }
  return true;
}

bool CollisionSystem::should_collide(const Collider *a, const Collider *b) const {
  bool a_collides_with_b = has_layer(a->mask, b->collisionLayer);
  bool b_collides_with_a = has_layer(b->mask, a->collisionLayer);

  return a_collides_with_b && b_collides_with_a;
}

void CollisionSystem::add_collision_callback(CollisionCallback callback) {
  collision_callbacks_.push_back(callback);
}

void CollisionSystem::clear_collision_callbacks() {
  collision_callbacks_.clear();
}

void CollisionSystem::debug_draw() const {

  for (Entity const* entity : entities_) {
    if (!is_valid_entity(entity)) {
      continue;
    }

    auto const *transform = entity->get_component<Components::Transform>();
    auto const *collider = entity->get_component<Components::Collider>();

    if (collider->type == ColliderType::CIRCLE) {
      DrawCircleLines(
        static_cast<int>(transform->position.x),
         static_cast<int>(transform->position.y),
         collider->radius,
         collider->debug_color);
    } else {
      const Rectangle rect = {
        transform->position.x - collider->size.x / 2.0f,
        transform->position.y - collider->size.y / 2.0f,
        collider->size.x,
        collider->size.y
      };
      DrawRectangleLinesEx(rect, 1.f, collider->debug_color);
    }
  }
}

void CollisionSystem::clear_entities() {
  entities_.clear();
  TRACELOG(LOG_INFO, "CollisionSystem cleared all entities");
}

bool CollisionSystem::is_valid_entity(const Entity *entity) const {
  return entity && entity->has_component<Components::Transform>() && entity->has_component<Collider>();
}

void CollisionSystem::resolve_collision(const CollisionInfo &info) {
  auto const* collider_a = info.entity_a->get_component<Collider>();
  auto const* collider_b = info.entity_b->get_component<Collider>();

  if (collider_a->is_trigger || collider_b->is_trigger) {
    return;
  }

  auto* transform_a = info.entity_a->get_component<Components::Transform>();
  auto* transform_b = info.entity_b->get_component<Components::Transform>();

  bool is_a_moving = transform_a->velocity.x != 0.0f || transform_a->velocity.y != 0.0f;
  bool is_b_moving = transform_b->velocity.x != 0.0f || transform_b->velocity.y != 0.0f;

  if (is_a_moving && !is_b_moving) {
    transform_a->position.x -= info.collision_normal.x * info.penetration_depth;
    transform_a->position.y -= info.collision_normal.y * info.penetration_depth;
    transform_a->velocity = {0,0};
  } else if (!is_a_moving && is_b_moving) {
    transform_b->position.x += info.collision_normal.x * info.penetration_depth;
    transform_b->position.y += info.collision_normal.y * info.penetration_depth;
    transform_b->velocity = {0,0};
  } else if (is_a_moving && is_b_moving) {
    float half_penetration = info.penetration_depth / 2.f;
    transform_a->position.x -= info.collision_normal.x * half_penetration;
    transform_a->position.y -= info.collision_normal.y * half_penetration;
    transform_b->position.x += info.collision_normal.x * half_penetration;
    transform_b->position.y += info.collision_normal.y * half_penetration;
  }
}








