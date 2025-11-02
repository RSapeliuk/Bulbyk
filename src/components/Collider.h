//
// Created by helpe on 22.10.2025.
//
#pragma once
#include "raylib.h"
#include "core/Entity.h"

namespace Components {
  enum class ColliderType {
    CIRCLE,
    RECTANGLE
  };

  enum class CollisionLayer : unsigned int {
    NONE = 0,
    PLAYER = 1 << 0,
    ENEMY = 1 << 1,
    BULLET = 1 << 2,
    OBSTACLE = 1 << 3,
    PICKUP = 1 << 4,
    ALL = 0xFFFFFFFF
  };

  inline CollisionLayer operator|(CollisionLayer a, CollisionLayer b) {
    return static_cast<CollisionLayer>(static_cast<unsigned int>(a) | static_cast<unsigned int>(b));
  }

  inline CollisionLayer operator&(CollisionLayer a, CollisionLayer b) {
    return static_cast<CollisionLayer>(static_cast<unsigned int>(a) & static_cast<unsigned int>(b));
  }

  inline bool has_layer(CollisionLayer mask, CollisionLayer layer) {
    return (mask & layer) == layer;
  }

  struct Collider : public Component {
    ColliderType type = ColliderType::CIRCLE;

    float radius = 10.0f;

    Vector2 offset = {0.0f, 0.0f};
    Vector2 size = {20.0f, 20.0f};

    CollisionLayer collisionLayer = CollisionLayer::NONE;
    CollisionLayer mask = CollisionLayer::ALL;

    bool is_trigger = false;

    Color debug_color = GREEN;

    Collider() = default;

    Collider(float r, CollisionLayer layer_type, bool trigger = false)
      : type(ColliderType::CIRCLE)
        , radius(r)
        , collisionLayer(layer_type)
        , is_trigger(trigger) {
    }

    Collider(Vector2 size, CollisionLayer layer_type, bool trigger = false)
      : type(ColliderType::RECTANGLE)
        , size(size)
        , collisionLayer(layer_type)
        , is_trigger(trigger) {
    }
  };
}
