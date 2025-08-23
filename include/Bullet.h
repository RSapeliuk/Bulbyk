#pragma once
#include <raylib.h>

class Bullet {
private:
  Vector2 position_;
  Vector2 velocity_;
  float speed_;
  float damage_;
  float radius_;
  bool active_;
  Color color_;
  float lifetime_;
  float max_lifetime_;

public:
  Bullet(Vector2 start_pos, Vector2 target_pos, float speed, float damage);
  ~Bullet() = default;

  // Rule of 5
  Bullet(const Bullet&) = delete;
  Bullet& operator=(const Bullet&) = delete;
  Bullet(Bullet&&) = default;
  Bullet& operator=(Bullet&&) = default;

  void update();
  void draw() const;
  void deactivate() { active_ = false; }

  // Getters
  [[nodiscard]] bool is_active() const noexcept { return active_; }
  [[nodiscard]] Vector2 get_position() const noexcept { return position_; }
  [[nodiscard]] float get_damage() const noexcept { return damage_; }
  [[nodiscard]] Rectangle get_bounds() const noexcept;

private:
  void check_screen_bounds();
  Vector2 calculate_direction(Vector2 start, Vector2 target) const;
};
