#include "PlayerCamera.h"

#include <algorithm>
#include <random>

PlayerCamera::PlayerCamera(Vector2 screen_size, Vector2 world_size, float follow_speed)
  : world_size_{world_size}
  , screen_size_{screen_size}
  , follow_speed_{follow_speed}
  , offset_{screen_size.x / 2.f, screen_size.y / 2.f} {

  camera_.target = {world_size.x / 2.f, world_size.y / 2.f};
  camera_.offset = offset_;
  camera_.rotation = 0.f;
  camera_.zoom = 1.f;
}

void PlayerCamera::update(Vector2 target_pos) {
  const auto delta_time = GetFrameTime();

  Vector2 desired_target = target_pos;

  camera_.target.x += (desired_target.x - camera_.target.x) * follow_speed_ * delta_time;
  camera_.target.y += (desired_target.y - camera_.target.y) * follow_speed_ * delta_time;

  clamp_to_world_bounds();

  if (shake_timer_ > 0.f) {
    shake_timer_ -= delta_time;

    static std::random_device rd_;
    static std::mt19937 gen_{rd_()};
    static std::uniform_real_distribution<float> shake_dist_{-shake_intensity_, shake_intensity_};

    camera_.offset.x += shake_dist_(gen_);
    camera_.offset.y += shake_dist_(gen_);
  } else {
    camera_.offset = offset_;
  }
}

void PlayerCamera::clamp_to_world_bounds() {
  const float half_screen_width = screen_size_.x / (2.f * camera_.zoom);
  const float half_screen_height = screen_size_.y / (2.f * camera_.zoom);

  camera_.target.x = std::clamp(camera_.target.x, offset_.x - half_screen_width, offset_.x + world_size_.x - half_screen_width);
  camera_.target.y = std::clamp(camera_.target.y, offset_.y - half_screen_height, offset_.y + world_size_.y - half_screen_height);
}

void PlayerCamera::begin_mode() const {
  BeginMode2D(camera_);
}

void PlayerCamera::end_mode() const {
  EndMode2D();
}

Vector2 PlayerCamera::screen_to_world(Vector2 screen_pos) const {
  return GetScreenToWorld2D(screen_pos, camera_);
}

Vector2 PlayerCamera::world_to_screen(Vector2 world_pos) const {
  return GetWorldToScreen2D(world_pos, camera_);
}

Rectangle PlayerCamera::get_camera_bounds() const {
  const float half_w = screen_size_.x / (2.0f * camera_.zoom);
  const float half_h = screen_size_.y / (2.0f * camera_.zoom);
  return Rectangle{
    camera_.target.x - half_w,
    camera_.target.y - half_h,
    half_w * 2.0f,
    half_h * 2.0f
  };
}

void PlayerCamera::set_zoom(float zoom) {
  camera_.zoom = std::clamp(zoom, 0.5f, 3.f);
}

void PlayerCamera::shake(float intensity, float duration) {
  shake_timer_ = duration;
  shake_intensity_ = intensity;
}


