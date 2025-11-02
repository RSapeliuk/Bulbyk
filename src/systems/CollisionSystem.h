//
// Created by helpe on 22.10.2025.
//

#ifndef BULBYK_COLLISIONSYSTEM_H
#define BULBYK_COLLISIONSYSTEM_H
#include <functional>

#include "raylib.h"
#include "components/Collider.h"
#include "core/Entity.h"


struct CollisionInfo {
  Entity *entity_a;
  Entity *entity_b;
  Vector2 collision_point;
  Vector2 collision_normal;
  float penetration_depth;
};

using CollisionCallback = std::function<void(const CollisionInfo&)>;

class CollisionSystem {
private:
  std::vector<Entity*> entities_;
  std::vector<CollisionCallback> collision_callbacks_;

public:
  CollisionSystem() = default;

  void register_entity(Entity* entity);
  void unregister_entity(const Entity* entity);
  void clear_entities();

  void update();

  void add_collision_callback(CollisionCallback callback);
  void clear_collision_callbacks();

  bool check_collision(Entity* entity_a, Entity* entity_b, CollisionInfo* out_info = nullptr) const;

  void resolve_collision (const CollisionInfo& info);

  void debug_draw() const;

private:
  bool is_valid_entity(const Entity* entity) const;
  bool should_collide(const Components::Collider* a, const Components::Collider* b) const;

  bool circle_vs_circle( Vector2 pos_a, float radius_a, Vector2 pos_b, float radius_b, CollisionInfo* out_info ) const;

  bool rect_vs_rect(Vector2 pos_a, Vector2 size_a, Vector2 pos_b, Vector2 size_b, CollisionInfo* out_info) const;

  bool circle_vs_rect(Vector2 pos_a, float radius_a, Vector2 pos_b, Vector2 size_b, CollisionInfo* out_info) const;
};


#endif //BULBYK_COLLISIONSYSTEM_H