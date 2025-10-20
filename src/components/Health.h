#pragma once
#include "../core/Entity.h"

namespace Components {

struct Health final : public Component {
  int current_health = 100;
  int max_health = 100;
  bool invulnerable = false;
  float last_damage_time = 0.f;

  Health() = default;

  explicit Health(const int max_hp) : current_health(max_hp), max_health(max_hp) {
  }

  Health(const int current, const int max) : current_health(current), max_health(max) {
  }

  bool is_alive() const { return current_health > 0;}

  float get_health_percentage() const { return max_health > 0 ? static_cast<float>(current_health) / max_health : 0.f; }

  bool is_full_health() const { return current_health >= max_health; }

  bool is_low_health(const float threshold = 0.25f) const { return get_health_percentage() <= threshold; }
};

} // namespace Components
