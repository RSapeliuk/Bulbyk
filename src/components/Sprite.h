#pragma once
#include "raylib.h"
#include "../core/Entity.h"

namespace Components {

struct Sprite final : Component {
  std::string texture_name = "";
  Rectangle source_rect = {0, 0, 0, 0};
  Vector2 origin = {0.5f, 0.5f};
  Color tint = WHITE;
  bool visible = true;
  int layer = 0;

  bool use_primitive = true;
  float primitive_radius = 10.f;
  Color primitive_color = WHITE;

  Sprite() = default;

  explicit Sprite(const std::string &texture_name) : texture_name(texture_name), use_primitive(false) {
  }

  Sprite(const float radius, const Color color) : use_primitive(true), primitive_radius(radius), primitive_color(color)  {
  }

  Sprite(const float radius, const Color color, const int render_layer)
      : layer(render_layer), use_primitive(true), primitive_radius(radius), primitive_color(color) {
  }
};

} // namespace Components