#pragma once
#include <memory>
#include <random>

#include "Enemy.h"

class EnemyFactory {
private:
  static std::random_device rd_;
  static std::mt19937 gen_;

public:
  static std::unique_ptr<Enemy> create_enemy(EnemyType type, Vector2 position);

  static std::unique_ptr<Enemy> create_random_enemy_for_level(int level, Vector2 position);

private:

  static EnemyType get_random_type_for_level(int level);
};