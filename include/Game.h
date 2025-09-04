#pragma once
#include "GameState.h"
#include <memory>
#include <vector>
#include <string>

#include "EnemyFactory.h"
#include "PlayerCamera.h"
#include "Constants.h"

class Player;
class Enemy;
class Bullet;

class Game {
public:

  explicit Game(std::string title = "🌾 БУЛЬБИК: ДО КОРЕНЯ ЗЛА");

  ~Game();
  Game(const Game&) = delete;
  Game& operator=(const Game&) = delete;
  Game(Game&&) = default;
  Game& operator=(Game&&) = default;

  void run();

private:
  // Методи життєвого циклу
  void init();
  void update();
  void draw();

  void draw_world_background() const;

  void draw_game_objects();
  void draw_state_messages() const;
  void draw_ui() const;
  void draw_minimap() const;
  void draw_world_bounds() const;
  bool is_position_in_camera(Vector2 position, float margin = 100.f) const;
  void toggle_language();

  int get_max_enemies() const;

  void unload();

  // Ігрова логіка
  void spawn_enemy();
  void spawn_bullet();
  void handle_input();
  void check_collisions();
  void cleanup_dead_objects();
  void update_timers();

  void restart_game();

  Enemy* find_nearest_enemy() const;
  Vector2 get_random_spawn_position() const;

  void update_difficulty();


#ifdef _DEBUG
  bool show_debug_info_ = true;
  void draw_debug_info() const;
#endif

  // Основні об'єкти гри
  std::string title_;
  GameState state_ = GameState::PLAYING;

  // Smart pointers замість raw
  std::unique_ptr<Player> player_;
  std::vector<std::unique_ptr<Enemy>> enemies_;
  std::vector<std::unique_ptr<Bullet>> bullets_;
  std::unique_ptr<PlayerCamera> camera_;

  // Таймери і лічильники з ініціалізацією
  float spawn_timer_ = 0.0f;
  float spawn_interval_ = GameConstants::Gameplay::DEFAULT_SPAWN_INTERVAL;
  float shoot_timer_ = 0.0f;
  float shoot_interval_ = GameConstants::Gameplay::DEFAULT_SHOOT_INTERVAL;
  int kill_count_ = 0;
  float game_time_ = 0.0f;
};
