#pragma once
#include <vector>
#include "../components/Transform.h"
#include "raylib.h"

class Entity;

class TransformSystem {
private:
  std::vector<Entity *> entities_;

public:
  void register_entity(Entity *entity);
  void unregister_entity(const Entity *entity);

  void update(float delta_time);

  static void set_position(const Entity *entity, Vector2 position);

  static void move(const Entity *entity, Vector2 offset);

  static void set_velocity(const Entity *entity, Vector2 velocity);

  static Vector2 get_position(const Entity *entity);

  static void clamp_to_world_bounds(const Entity *entity, Rectangle world_bounds);

  void clear_entities();

private:
  static bool is_valid_entity(const Entity *entity);
};
