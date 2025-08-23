#pragma once
#include <raylib.h>

class Bullet {
public:
  Bullet(const Vector2& pos, const Vector2& dir, float speed, float size);
  ~Bullet() = default;

  void Update();
  void Draw() const;
  Rectangle GetBounds() const;
  bool IsAlive() const;

private:
  Vector2 position;
  Vector2 direction;
  float speed;
  float size;
  bool alive;
  Color color;
};
