#include "ColoradoBeetle.h"

#include <algorithm>


ColoradoBeetle::ColoradoBeetle(Vector2 position)
  : Enemy(position, 30.f,150.f,10.f,15.f)
  , color_(ORANGE)
{}

void ColoradoBeetle::update(const Vector2 &targetPos) {

  const auto delta_time = GetFrameTime();
  move_towards(targetPos, delta_time);

  attackCooldown_ = std::max(0.f, attackCooldown_ - delta_time);

  if (attackCooldown_ <= 0.f) {
    attack_logic();
    attackCooldown_ = ATTACK_INTERVAL;
  }
}

void ColoradoBeetle::draw() const {
  DrawCircleV(position_, radius_, color_);

  DrawCircleV({position_.x - 5, position_.y - 3}, 2, BLACK);
  DrawCircleV({position_.x + 5, position_.y - 3}, 2, BLACK);
  DrawCircleV({position_.x, position_.y + 5}, 3, DARKBROWN);

  // Індикатор здоров'я
  const float health_bar_width = radius_ * 2.0f;
  const float health_bar_height = 3.0f;
  const float health_percentage = health_ / max_health_;

  // Фон health bar
  DrawRectangle(
      static_cast<int>(position_.x - health_bar_width/2),
      static_cast<int>(position_.y - radius_ - 10),
      static_cast<int>(health_bar_width),
      static_cast<int>(health_bar_height),
      RED
  );

  // Актуальне здоров'я
  DrawRectangle(
      static_cast<int>(position_.x - health_bar_width/2),
      static_cast<int>(position_.y - radius_ - 10),
      static_cast<int>(health_bar_width * health_percentage),
      static_cast<int>(health_bar_height),
      GREEN
  );
}

void ColoradoBeetle::attack_logic() {

}



