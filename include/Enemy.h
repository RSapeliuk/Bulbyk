#pragma once
#include <raylib.h>

enum class EnemyType {
  ColoradoBeetle,
  Aphid,
  Wireworm,
  MoleCricket,
  Cutworm
};

class Enemy {

protected:
  Vector2 position_;
  Vector2 velocity_;
  float health_;
  float max_health_;
  float speed_;
  float damage_;
  float radius_;
  bool active_ = true;

public:

  virtual ~Enemy() = default;

  virtual void update(const Vector2 &targetPos) = 0;
  virtual void draw() const = 0;
  virtual EnemyType get_type() const = 0;

  virtual void take_damage(float damage);
  virtual Rectangle get_bounds() const;

  [[nodiscard]] bool is_alive() const noexcept { return health_ > 0 && active_; }
  [[nodiscard]] Vector2 get_position() const noexcept { return position_; }
  [[nodiscard]] float get_damage() const noexcept { return damage_; }


protected:
  Enemy(Vector2 position, float health, float speed, float damage, float radius);
  void move_towards(const Vector2& target, float delta_time);

};
