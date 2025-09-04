#include "Game.h"

#include <algorithm>
#include <iostream>
#include <random>

#include "Player.h"
#include "Enemy.h"
#include "Bullet.h"
#include "ColoradoBeetle.h"
#include "TextUtils.h"

Game::~Game() = default;

Game::Game(std::string title)
  : title_{std::move(title)}
    , state_{GameState::PLAYING}
    , player_(nullptr)
    , enemies_{}
    , bullets_{} {
  std::cout << "🎮 Створюємо гру: " << title_ << std::endl;
}

void Game::init() {
  InitWindow(GameConstants::SCREEN_WIDTH, GameConstants::SCREEN_HEIGHT, title_.c_str());
  SetTargetFPS(60);
  TextUtils::init_translations();
  InitAudioDevice();

  camera_ = std::make_unique<PlayerCamera>(
    Vector2{static_cast<float>(GameConstants::SCREEN_WIDTH), static_cast<float>(GameConstants::SCREEN_HEIGHT)},
    Vector2{static_cast<float>(GameConstants::WORLD_WIDTH), static_cast<float>(GameConstants::WORLD_HEIGHT)});

  Vector2 player_start_pos = {
    static_cast<float>(GameConstants::WORLD_WIDTH) / 2.f,
    static_cast<float>(GameConstants::WORLD_HEIGHT) / 2.f
  };

  player_ = std::make_unique<Player>(player_start_pos);
}

void Game::update() {
  const float dt = GetFrameTime();

  switch (state_) {
    case GameState::PLAYING:
      update_timers();
      game_time_ += dt;
      static float difficulty_timer = 5.f;
      difficulty_timer -= dt;
      handle_input();
      if (difficulty_timer <= 0.f) {
        difficulty_timer = 5.f;
        update_difficulty();
      }

      if (player_) {
        camera_->update(player_->get_position());
      }

      if (player_ && player_->is_alive()) {
        player_->update();
      } else if (state_ == GameState::PLAYING) {
        state_ = GameState::GAMEOVER;
      }

      if (spawn_timer_ <= 0.f) {
        spawn_enemy();
        spawn_timer_ = spawn_interval_;
      }

      if (shoot_timer_ <= 0.f && !enemies_.empty()) {
        spawn_bullet();
      }

      for (auto &e: enemies_) {
        if (e && e->is_alive()) {
          e->update(player_->get_position());
        }
      }

      for (auto &b: bullets_) {
        if (b && b->is_active()) {
          b->update();
        }
      }

      check_collisions();
      cleanup_dead_objects();
      break;
    case GameState::BOSS:
      break;
    case GameState::PAUSE:
      handle_input();
      break;
    case GameState::GAMEOVER:
      handle_input();
      break;
    default:
      break;
  }
}

void Game::draw() {
  BeginDrawing();
  Color background_color;
  switch (state_) {
    case GameState::PLAYING:
      background_color = Color{34, 139, 34, 255};
      break;
    case GameState::PAUSE:
      background_color = Color{64, 64, 64, 255};
      break;
    case GameState::GAMEOVER:
      background_color = Color{139, 34, 34, 255};
      break;
    default:
      background_color = BLACK;
  }

  ClearBackground(background_color);

  if (state_ == GameState::PLAYING || state_ == GameState::PAUSE) {
    camera_->begin_mode();
    draw_world_background();
    draw_game_objects();
    draw_world_bounds();
    camera_->end_mode();
  }

  draw_ui();
  draw_minimap();

  EndDrawing();
}

void Game::draw_world_background() const {
  DrawRectangle(0, 0, GameConstants::WORLD_WIDTH, GameConstants::WORLD_HEIGHT, Color{34, 139, 34, 255});

  const int grid_size = 64;
  for (int x = 0; x < GameConstants::WORLD_WIDTH; x += grid_size) {
    for (int y = 0; y < GameConstants::WORLD_HEIGHT; y += grid_size) {
      DrawRectangleLines(x, y, grid_size, grid_size, ColorAlpha(BLACK, 0.1f));
    }
  }
}

void Game::draw_world_bounds() const {
  DrawRectangleLines(0, 0, GameConstants::WORLD_WIDTH, GameConstants::WORLD_HEIGHT, RED);

  constexpr int danger_zone = 20;

  DrawRectangleLinesEx(Rectangle{
                         danger_zone, danger_zone, GameConstants::WORLD_WIDTH - danger_zone * 2, GameConstants::WORLD_HEIGHT - danger_zone * 2
                       }, 4.f, DARKPURPLE);
}

void Game::draw_minimap() const {
  constexpr int minimap_size = 150;
  const int minimap_x = GameConstants::SCREEN_WIDTH - minimap_size - 10;
  const int minimap_y = 10;

  DrawRectangle(minimap_x, minimap_y, minimap_size, minimap_size, ColorAlpha(BLACK, 0.7f));
  DrawRectangleLines(minimap_x, minimap_y, minimap_size, minimap_size, Color{255, 255, 255, 255});

  constexpr float scale_x = static_cast<float>(minimap_size) / GameConstants::WORLD_WIDTH;
  constexpr float scale_y = static_cast<float>(minimap_size) / GameConstants::WORLD_HEIGHT;

  if (player_) {
    const auto pos = player_->get_position();
    DrawCircle(minimap_x + static_cast<int>(pos.x * scale_x), minimap_y + static_cast<int>(pos.y * scale_y), 3.f, BLUE);
  }

  for (const auto& e: enemies_) {
    if (e && e->is_alive()) {
      const auto pos = e->get_position();
      DrawCircle(minimap_x + static_cast<int>(pos.x * scale_x), minimap_y + static_cast<int>(pos.y * scale_y), 1.f, RED);
    }
  }

  auto camera_bounds = camera_->get_camera_bounds();
  DrawRectangleLines(
    minimap_x + static_cast<int>(camera_bounds.x * scale_x),
    minimap_y + static_cast<int>(camera_bounds.y * scale_y),
     static_cast<int>(camera_bounds.width * scale_x),
     static_cast<int>(camera_bounds.height * scale_y),
     YELLOW);
}

void Game::draw_game_objects() {
  if (player_) {
    player_->draw();
  }

  for (auto &e: enemies_) {
    if (e && e->is_alive()) {
      e->draw();
    }
  }

  for (auto &b: bullets_) {
    if (b && b->is_active()) {
      b->draw();
    }
  }
}

void Game::draw_ui() const {
  const int ui_margin = 10;
  const int ui_font_size = 20;
  const int ui_line_height = 25;

  int y_offset = ui_margin;

  // Здоров'я гравця
  const int health = (player_ && player_->is_alive()) ? player_->get_health() : 0;
  DrawText(TextFormat("%s: %d", TextUtils::get_text("health"), health),
           ui_margin, y_offset, ui_font_size, WHITE);
  y_offset += ui_line_height;

  // Статистика
  DrawText(TextFormat("%s: %d", TextUtils::get_text("killed"), kill_count_),
           ui_margin, y_offset, ui_font_size, WHITE);
  y_offset += ui_line_height;

  DrawText(TextFormat("%s: %.1f %s", TextUtils::get_text("time"), game_time_, TextUtils::get_text("seconds")),
           ui_margin, y_offset, ui_font_size, WHITE);
  y_offset += ui_line_height;

  DrawText(TextFormat("%s: %zu", TextUtils::get_text("enemies"), enemies_.size()),
           ui_margin, y_offset, ui_font_size, WHITE);
  y_offset += ui_line_height;

  DrawText(TextFormat("%s: %zu", TextUtils::get_text("bullets"), bullets_.size()),
           ui_margin, y_offset, ui_font_size, WHITE);

  draw_state_messages();

#ifdef _DEBUG
  draw_debug_info();
#endif
}

void Game::draw_state_messages() const {
  const int center_x = GameConstants::SCREEN_WIDTH / 2;
  const int center_y = GameConstants::SCREEN_HEIGHT / 2;

  switch (state_) {
    case GameState::PAUSE: {
      TextUtils::draw_text_centered("paused", center_x, center_y - 20, 40, YELLOW);
      TextUtils::draw_text_centered("continue_hint", center_x, center_y + 30, 20, WHITE);
      TextUtils::draw_text_centered("language_switch", center_x, center_y + 60, 16, LIGHTGRAY);
      break;
    }

    case GameState::GAMEOVER: {
      TextUtils::draw_text_centered("game_over", center_x, center_y - 25, 50, RED);
      TextUtils::draw_text_centered("restart_hint", center_x, center_y + 40, 25, WHITE);

      const char *stats_format = TextUtils::get_text("survival_stats");
      const char *final_stats = TextFormat(stats_format, game_time_, kill_count_);
      const int stats_width = MeasureText(final_stats, 20);
      DrawText(final_stats, center_x - stats_width / 2, center_y + 80, 20, LIGHTGRAY);

      TextUtils::draw_text_centered("language_switch", center_x, center_y + 110, 16, LIGHTGRAY);
      break;
    }

    case GameState::PLAYING: {
      if (game_time_ < 10.0f) {
        TextUtils::draw_text_localized("move_controls",
                                       10, GameConstants::SCREEN_HEIGHT - 80, 16, LIGHTGRAY);
        TextUtils::draw_text_localized("auto_shoot_hint",
                                       10, GameConstants::SCREEN_HEIGHT - 60, 16, LIGHTGRAY);
        TextUtils::draw_text_localized("language_switch",
                                       10, GameConstants::SCREEN_HEIGHT - 40, 16, LIGHTGRAY);
      }
      break;
    }
    default: break;
  }
}

#ifdef _DEBUG
void Game::draw_debug_info() const {
  if (!show_debug_info_) return;

  const int debug_x = SCREEN_WIDTH - 250;
  int debug_y = 10;
  const int line_height = 20;

  TextUtils::draw_text_localized("debug_info", debug_x, debug_y, 16, YELLOW);
  debug_y += line_height;

  DrawText(TextFormat("%s: %d", TextUtils::get_text("fps"), GetFPS()),
           debug_x, debug_y, 16, GREEN);
  debug_y += line_height;

  DrawText(TextFormat("%s: %.3f", TextUtils::get_text("delta"), GetFrameTime()),
           debug_x, debug_y, 16, GREEN);
  debug_y += line_height;

  DrawText(TextFormat("%s: %.2f", TextUtils::get_text("spawn_timer"), spawn_timer_),
           debug_x, debug_y, 16, WHITE);
  debug_y += line_height;

  DrawText(TextFormat("%s: %.2f", TextUtils::get_text("shoot_timer"), shoot_timer_),
           debug_x, debug_y, 16, WHITE);
  debug_y += line_height;

  DrawText(TextFormat("%s: %.2f", TextUtils::get_text("spawn_interval"), spawn_interval_),
           debug_x, debug_y, 16, ORANGE);
  debug_y += line_height;

  DrawText(TextFormat("%s: %d", TextUtils::get_text("max_enemies"), get_max_enemies()),
           debug_x, debug_y, 16, ORANGE);
  debug_y += line_height;

  // Показуємо позицію гравця
  if (player_) {
    auto pos = player_->get_position();
    DrawText(TextFormat("%s: (%.0f, %.0f)", TextUtils::get_text("player_pos"), pos.x, pos.y),
             debug_x, debug_y, 16, BLUE);
  }
}
#endif

void Game::run() {
  init();
  while (!WindowShouldClose()) {
    update();
    draw();
  }

  unload();
}

void Game::unload() {
  if (player_) {
    player_ = nullptr;
  }

  enemies_.clear();
  bullets_.clear();
}

Vector2 Game::get_random_spawn_position() const {
  static std::random_device rd;
  static std::mt19937 gen{rd()};

  auto camera_bounds = camera_->get_camera_bounds();
  const float margin = 100.f;
  const float spawn_left = std::max(0.f, camera_bounds.x - margin);
  const float spawn_right = std::min(static_cast<float>(GameConstants::WORLD_WIDTH), camera_bounds.x + camera_bounds.width + margin);
  const float spawn_top = std::max(0.f, camera_bounds.y - margin);
  const float spawn_bottom = std::min(static_cast<float>(GameConstants::WORLD_HEIGHT), camera_bounds.y + camera_bounds.height + margin);

  std::uniform_int_distribution<int> side_dist(0, 3);

  switch (side_dist(gen)) {
    case 0: {
      // Зверху
      std::uniform_real_distribution<float> x_dist(spawn_left, spawn_right);
      return Vector2{x_dist(gen), spawn_top};
    }
    case 1: {
      // Справа
      std::uniform_real_distribution<float> y_dist(spawn_top, spawn_bottom);
      return Vector2{spawn_right, y_dist(gen)};
    }
    case 2: {
      // Знизу
      std::uniform_real_distribution<float> x_dist(spawn_left, spawn_right);
      return Vector2{x_dist(gen), spawn_bottom};
    }
    case 3: {
      // Зліва
      std::uniform_real_distribution<float> y_dist(spawn_top, spawn_bottom);
      return Vector2{spawn_left, y_dist(gen)};
    }
    default:
      return Vector2{0, spawn_top};
  }
}

void Game::update_difficulty() {
  constexpr float difficulty_time = 30.f;
  const int difficulty_level = static_cast<int>(game_time_ / difficulty_time) + 1;

  spawn_interval_ = std::max(0.5f, GameConstants::Gameplay::DEFAULT_SPAWN_INTERVAL - (difficulty_level * 0.2f));

  shoot_interval_ = std::max(0.05f, GameConstants::Gameplay::DEFAULT_SHOOT_INTERVAL - (difficulty_level * 0.02f));
}

void Game::spawn_enemy() {
  if (static_cast<int>(enemies_.size()) >= get_max_enemies()) return;

  auto spawn_pos = get_random_spawn_position();

  auto new_enemy = std::make_unique<ColoradoBeetle>(spawn_pos);
  enemies_.push_back(std::move(new_enemy));
}

void Game::spawn_bullet() {
  if (!player_ || !player_->is_alive()) return;

  auto *target = find_nearest_enemy();
  if (!target) return;

  auto bullet = std::make_unique<Bullet>(player_->get_position(), target->get_position(), 300.f, 25.f);

  bullets_.push_back(std::move(bullet));
  shoot_timer_ = shoot_interval_;
}

Enemy *Game::find_nearest_enemy() const {
  if (enemies_.empty() || !player_) return nullptr;

  const auto player_pos = player_->get_position();
  float min_distance_sq = std::numeric_limits<float>::max();
  Enemy *nearest = nullptr;

  for (const auto &e: enemies_) {
    if (!e || !e->is_alive()) continue;

    const auto [x, y] = e->get_position();
    const float dx = player_pos.x - x;
    const float dy = player_pos.y - y;

    if (const float distance = dx * dx + dy * dy; distance < min_distance_sq) {
      min_distance_sq = distance;
      nearest = e.get();
    }
  }

  return nearest;
}

void Game::update_timers() {
  const auto delta_time = GetFrameTime();

  spawn_timer_ = std::max(0.f, spawn_timer_ - delta_time);
  shoot_timer_ = std::max(0.f, shoot_timer_ - delta_time);
}

void Game::restart_game() {
  player_ = std::make_unique<Player>(Vector2{
    static_cast<float>(GameConstants::WORLD_WIDTH) / 2.f,
    static_cast<float>(GameConstants::WORLD_HEIGHT) / 2.f
  });
  enemies_.clear();
  bullets_.clear();
  spawn_timer_ = spawn_interval_;
  kill_count_ = 0;
  game_time_ = 0.0f;
  shoot_timer_ = 0.0f;
  state_ = GameState::PLAYING;
  spawn_interval_ = GameConstants::Gameplay::DEFAULT_SPAWN_INTERVAL;
  shoot_interval_ = GameConstants::Gameplay::DEFAULT_SHOOT_INTERVAL;

  state_ = GameState::PLAYING;
}

void Game::handle_input() {
  if (IsKeyPressed(KEY_P)) {
    state_ = (state_ == GameState::PLAYING) ? GameState::PAUSE : GameState::PLAYING;
  }

  if (IsKeyPressed(KEY_L)) {
    toggle_language();
  }

  if (IsKeyPressed(KEY_R) && state_ == GameState::GAMEOVER) {
    restart_game();
  }
#ifdef _DEBUG
  // F1 - toggle debug info
  if (IsKeyPressed(KEY_F1)) {
    show_debug_info_ = !show_debug_info_;
  }
#endif
}

void Game::toggle_language() {
  Language current = TextUtils::get_current_language();
  Language new_lang = (current == Language::English) ? Language::Ukrainian : Language::English;
  TextUtils::set_language(new_lang);
}

int Game::get_max_enemies() const {
  const int base_max = 20;
  const int difficulty_level = static_cast<int>(game_time_ / 30.f) + 1;
  return base_max + (difficulty_level * 5);
}

void Game::check_collisions() {
  if (!player_ || !player_->is_alive()) return;

  const auto player_bounds = player_->get_bounds();

  for (auto &e: enemies_) {
    if (!e || !e->is_alive()) continue;

    if (CheckCollisionRecs(player_bounds, e->get_bounds())) {
      player_->take_damage(static_cast<int>(e->get_damage()));
      e.reset();
      kill_count_++;
      break;
    }
  }
  for (auto &b: bullets_) {
    if (!b || !b->is_active()) continue;

    const auto bullet_bounds = b->get_bounds();

    for (auto &e: enemies_) {
      if (!e || !e->is_alive()) continue;

      if (CheckCollisionRecs(bullet_bounds, e->get_bounds())) {
        e->take_damage(b->get_damage());
        b->deactivate();

        if (!e->is_alive()) {
          e.reset();
          kill_count_++;
        }

        break;
      }
    }
  }
}

void Game::cleanup_dead_objects() {
  auto dead_enemies = std::remove_if(enemies_.begin(), enemies_.end(), [](auto &e) {
    return !e || !e->is_alive();
  });
  enemies_.erase(dead_enemies, enemies_.end());

  auto dead_bullets = std::remove_if(
    bullets_.begin(),
    bullets_.end(),
    [](const std::unique_ptr<Bullet> &bullet) {
      return !bullet || !bullet->is_active();
    }
  );
  bullets_.erase(dead_bullets, bullets_.end());
}
