#pragma once
#include <raylib.h>

class Player {
public:
  Player(float x, float y);

  ~Player() = default;

  void Update();

  void Draw() const;

  Vector2 GetPosition() const;
  Rectangle GetBounds() const;

private:
  Vector2 position;
  float speed;
  Color color;
  float size;
};
