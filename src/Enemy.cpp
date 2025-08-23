#include "Enemy.h"
#include <raylib.h>
#include <raymath.h>

Enemy::Enemy(const Vector2 &startPos, float speed, float size)
  : position(startPos)
  , speed(speed)
  , color(RED)
  , size(size)
{}

void Enemy::Update(const Vector2 &targetPos) {
  float dt = GetFrameTime();

  Vector2 dir = Vector2Subtract(targetPos, position);
  if (float len = Vector2Length(dir); len != 0.0f) {
    dir.x /= len;
    dir.y /= len;
  }

  position.x += dir.x * speed * dt;
  position.y += dir.y * speed * dt;
}

void Enemy::Draw() const {
  DrawRectangleV(
    {position.x - size/2, position.y - size/2},
    {size, size},
    color
    );
}

Rectangle Enemy::GetBounds() const {
  return {position.x - size/2, position.y - size/2, size, size};
}

