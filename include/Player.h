#pragma once
#include <raylib.h>

class Player {
public:
  explicit Player(Vector2 start_pos);

  ~Player() = default;

  Player(const Player &) = delete;
  Player& operator=(const Player&) = delete;
  Player(Player&&) = default;
  Player& operator=(Player&&) = default;

  void update();

  void draw() const;
  void take_damage(int damage);

  [[nodiscard]] Vector2 get_position() const noexcept {return position_;}
  [[nodiscard]] int get_health() const noexcept {return health_;}
  [[nodiscard]] Rectangle get_bounds() const noexcept;
  [[nodiscard]] bool is_alive() const noexcept {return health_ > 0;}

private:
  Vector2 position_;
  Vector2 velocity_;
  float speed_;
  float radius_;
  int health_;
  int maxHealth_;
  Color color_;
  float size_;
};
