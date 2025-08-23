#include "Bullet.h"

Bullet::Bullet(const Vector2 &pos, const Vector2 &dir, float speed, float size)
  : position(pos)
    , direction(dir)
    , speed(speed)
    , size(size)
    , alive(true)
    , color(BLUE) {
}


void Bullet::Update() {
  const float dt = GetFrameTime();
  position.x += direction.x * speed * dt;
  position.y += direction.y * speed * dt;

  if (position.x < -size || position.x > GetScreenWidth() + size ||
      position.y < -size || position.y > GetScreenHeight() + size) {
    alive = false;
  }
}

void Bullet::Draw() const {
  DrawCircleV(position, size/2, color);
}

Rectangle Bullet::GetBounds() const {
  return Rectangle{position.x - size/2, position.y - size/2, size, size};
}

bool Bullet::IsAlive() const {
  return alive;
}


