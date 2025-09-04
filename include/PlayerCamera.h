#pragma once
#include <raylib.h>

class PlayerCamera {
private:
  Camera2D camera_;
  Vector2 world_size_;
  Vector2 screen_size_;
  float follow_speed_;
  Vector2 offset_;

public:

  explicit PlayerCamera(Vector2 world_size, Vector2 screen_size, float follow_speed = 5.f);

  void update(Vector2 target_pos);
  void begin_mode() const;
  void end_mode() const;

  Vector2 screen_to_world(Vector2 screen_pos) const;

  Vector2 world_to_screen(Vector2 world_pos) const;

  Rectangle get_camera_bounds() const;

  Vector2 get_position() const { return camera_.target; }
  Vector2 get_offset() const { return camera_.offset; }
  float get_zoom() const { return camera_.zoom; }

  void set_zoom(float zoom);
  void shake(float intensity, float duration);

private:

  void clamp_to_world_bounds();

  float shake_timer_ = 0.f;
  float shake_intensity_ = 0.f;
};
