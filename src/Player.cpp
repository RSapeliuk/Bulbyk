#include "Player.h"
#include <algorithm>

Player::Player(Vector2 start_pos)
  : position_{start_pos}
    , velocity_{0.0f, 0.0f}
    , speed_(200.f)
    , radius_(20.f)
    , health_(100)
    , maxHealth_(100)
    , color_(BROWN)
    , size_(32.f) {
}

void Player::update() {
  auto delta_time = GetFrameTime();

  auto [input_x, input_y] = [this]() -> std::pair<float, float> {
    float x = 0.f, y = 0.f;
    if (IsKeyDown(KEY_W)) {
      y -= 1.f;
    }
    if (IsKeyDown(KEY_S)) {
      y += 1.f;
    }
    if (IsKeyDown(KEY_A)) {
      x -= 1.f;
    }
    if (IsKeyDown(KEY_D)) {
      x += 1.f;
    }
    return {x, y};
  }();

  auto normalize_diagonal = [](float &x, float &y) {
    if (x != 0.f && y != 0.f) {
      constexpr auto inv_sqrt2 = 0.707106781f;
      x *= inv_sqrt2;
      y *= inv_sqrt2;
    }
  };

  normalize_diagonal(input_x, input_y);

  velocity_ = Vector2{input_x, input_y};

  constexpr float WORLD_WIDTH = 2048.0f;
  constexpr float WORLD_HEIGHT = 1536.0f;

  position_.x += velocity_.x * speed_ * delta_time;
  position_.y += velocity_.y * speed_ * delta_time;

  position_.x = std::clamp(position_.x, radius_, WORLD_WIDTH - radius_);
  position_.y = std::clamp(position_.y, radius_, WORLD_HEIGHT - radius_);
}

void Player::draw() const {
  DrawCircleV(position_, radius_, color_);

  const auto eye_offset = 8.0f;
  const auto eye_y_offset = -5.0f;
  const auto eye_radius = 3.0f;

  DrawCircle(
    static_cast<int>(position_.x - eye_offset),
    static_cast<int>(position_.y + eye_y_offset),
    eye_radius,
    BLACK
  );
  DrawCircle(
    static_cast<int>(position_.x + eye_offset),
    static_cast<int>(position_.y + eye_y_offset),
    eye_radius,
    BLACK
  );

  // Індикатор здоров'я
  const float health_bar_width = radius_ * 2.0f;
  const float health_bar_height = 4.0f;
  const float health_percentage = static_cast<float>(health_) / static_cast<float>(maxHealth_);

  // Фон health bar
  DrawRectangle(
      static_cast<int>(position_.x - health_bar_width/2),
      static_cast<int>(position_.y - radius_ - 15),
      static_cast<int>(health_bar_width),
      static_cast<int>(health_bar_height),
      RED
  );

  // Актуальне здоров'я
  DrawRectangle(
      static_cast<int>(position_.x - health_bar_width/2),
      static_cast<int>(position_.y - radius_ - 15),
      static_cast<int>(health_bar_width * health_percentage),
      static_cast<int>(health_bar_height),
      GREEN
  );
}

void Player::take_damage(int damage) {
  health_ = std::max(0, health_ - damage);
}

Rectangle Player::get_bounds() const noexcept {
  return Rectangle{
    position_.x - radius_,
    position_.y - radius_,
    radius_ * 2.0f,
    radius_ * 2.0f
  };
}
