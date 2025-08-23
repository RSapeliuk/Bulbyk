#include "Enemy.h"
#include <raylib.h>
#include <raymath.h>

Enemy::Enemy(Vector2 position, float health, float speed, float damage, float radius)
  : position_{position}
    , velocity_{0.0f, 0.0f}
    , health_{health}
    , max_health_{health}
    , speed_{speed}
    , damage_{damage}
    , radius_{radius} {
}

Rectangle Enemy::get_bounds() const {
  return Rectangle{
    position_.x - radius_,
    position_.y - radius_,
    radius_ * 2.f,
    radius_ * 2.f
  };
}

void Enemy::take_damage(float damage) {
  health_ = std::max(0.f, health_ - damage);

  if (health_ <= 0.f) {
    active_ = false;
  }
}

void Enemy::move_towards(const Vector2 &target, float delta_time) {
  Vector2 direction = {
    target.x - position_.x,
    target.y - position_.y
  };

  float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);

  if (distance > radius_) {
    direction.x /= distance;
    direction.y /= distance;

    velocity_ = {
      direction.x * speed_,
      direction.y * speed_
    };

    position_.x += velocity_.x * delta_time;
    position_.y += velocity_.y * delta_time;
  } else {
    velocity_ = {0.f, 0.f};
  }
}
