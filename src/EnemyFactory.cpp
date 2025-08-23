#include "EnemyFactory.h"
#include "ColoradoBeetle.h"


std::random_device EnemyFactory::rd_;
std::mt19937 EnemyFactory::gen_{rd_()};

std::unique_ptr<Enemy> EnemyFactory::create_enemy(EnemyType type, Vector2 position) {
  switch (type) {
    case EnemyType::ColoradoBeetle:
      return std::make_unique<ColoradoBeetle>(position);
    default:
      return nullptr;
  }
}

std::unique_ptr<Enemy> EnemyFactory::create_random_enemy_for_level(int level, Vector2 position) {
  auto type = get_random_type_for_level(level);
  return create_enemy(type, position);
}

EnemyType EnemyFactory::get_random_type_for_level(int level) {
  return EnemyType::ColoradoBeetle;
}


