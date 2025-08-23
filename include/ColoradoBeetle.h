#pragma once
#include "Enemy.h"

class ColoradoBeetle : public Enemy {
private:
  Color color_;
  float attackCooldown_ = 0.f;
  static constexpr float ATTACK_INTERVAL = 1.f;

public:
  explicit ColoradoBeetle(Vector2 position);

  void update(const Vector2 &targetPos) override;
  void draw() const override;
  [[nodiscard]] EnemyType get_type() const override{ return EnemyType::ColoradoBeetle;}

private:
  void attack_logic();
};