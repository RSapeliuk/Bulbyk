#pragma once
#include "GameState.h"
#include <memory>
#include <vector>
#include <string>

#include "EnemyFactory.h"

class Player;
class Enemy;
class Bullet;

class Game {
public:
  // Конструктор з default параметром
  explicit Game(std::string title = "🌾 БУЛЬБИК: ДО КОРЕНЯ ЗЛА");

  // Rule of 5 - керуємо копіюванням і переміщенням
  ~Game();                          // Деструктор автоматичний
  Game(const Game&) = delete;                 // Заборона копіювання
  Game& operator=(const Game&) = delete;      // Заборона присвоєння
  Game(Game&&) = default;                     // Дозволяємо переміщення
  Game& operator=(Game&&) = default;          // Дозволяємо присвоєння переміщення

  // Головний метод запуску
  void run();

private:
  // Методи життєвого циклу
  void init();
  void update();
  void draw();

  void draw_game_objects();
  void draw_state_messages() const;
  void draw_ui() const;
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

  // Константи класу
  static constexpr int SCREEN_WIDTH = 1024;
  static constexpr int SCREEN_HEIGHT = 768;
  static constexpr float DEFAULT_SPAWN_INTERVAL = 0.3f;
  static constexpr float DEFAULT_SHOOT_INTERVAL = 0.2f;

  // Основні об'єкти гри
  std::string title_;
  GameState state_ = GameState::PLAYING;  // Ініціалізація відразу

  // Smart pointers замість raw
  std::unique_ptr<Player> player_;
  std::vector<std::unique_ptr<Enemy>> enemies_;
  std::vector<std::unique_ptr<Bullet>> bullets_;

  // Таймери і лічильники з ініціалізацією
  float spawn_timer_ = 0.0f;
  float spawn_interval_ = DEFAULT_SPAWN_INTERVAL;
  float shoot_timer_ = 0.0f;
  float shoot_interval_ = DEFAULT_SHOOT_INTERVAL;
  int kill_count_ = 0;
  float game_time_ = 0.0f;
};
