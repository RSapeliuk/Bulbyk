#pragma once
#include <raylib.h>

class Enemy {
public:
  Enemy(const Vector2 &startPos, float speed, float size);

  ~Enemy() = default;

  void Update(const Vector2 &targetPos);

  void Draw() const;

  Rectangle GetBounds() const;

private:
  Vector2 position;
  float speed;
  Color color;
  float size;
};
