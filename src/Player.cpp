#include "Player.h"
#include <algorithm>

Player::Player(float x, float y)
  : position({x,y})
  , speed(200.f)
  , color(GREEN)
  , size(32.f)
{}

void Player::Update() {
  if (IsKeyDown(KEY_W)) {
    position.y -= speed * GetFrameTime();
  }
  if (IsKeyDown(KEY_S)) {
    position.y += speed * GetFrameTime();
  }
  if (IsKeyDown(KEY_A)) {
    position.x -= speed * GetFrameTime();
  }
  if (IsKeyDown(KEY_D)) {
    position.x += speed * GetFrameTime();
  }

  position.x = std::clamp(position.x, size/2, GetScreenWidth() - size/2);
  position.y = std::clamp(position.y, size/2, GetScreenHeight() - size/2);
}

void Player::Draw() const {
  DrawRectangleV(
    {position.x - size/2, position.y - size/2},
    {size, size},
    color
    );
}

Vector2 Player::GetPosition() const {
  return position;
}

Rectangle Player::GetBounds() const {
  return {position.x - size/2, position.y - size/2, size, size};
}


