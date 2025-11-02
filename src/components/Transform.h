#pragma once
#include "raylib.h"
#include "../core/Entity.h"

namespace Components {
  struct Transform : Component {
    Vector2 position = {0.0f, 0.0f};
    float rotation = 0.0f;
    Vector2 scale = {1.0f, 1.0f};
    Vector2 velocity = {0.0f, 0.0f};

    Transform() = default;

    explicit Transform(const Vector2 position) : position(position) {
    }

    Transform(const Vector2 position, const float rotation) : position(position), rotation(rotation) {
    }

    Transform(const Vector2 position, const float rotation, const Vector2 scale) : position(position),
      rotation(rotation), scale(scale) {
    }
  };
} // namespace Components
