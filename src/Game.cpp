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
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, title_.c_str());
  SetTargetFPS(60);
  TextUtils::init_translations();
  InitAudioDevice();

  Vector2 player_start_pos = {
    static_cast<float>(SCREEN_WIDTH) / 2.f,
    static_cast<float>(SCREEN_HEIGHT) / 2.f
  };

  player_ = std::make_unique<Player>(player_start_pos);
}

void Game::update() {
  const float dt = GetFrameTime();
  game_time_ += dt;

  switch (state_) {
    case GameState::PLAYING:
      handle_input();
      update_timers();
      if (player_ && player_->is_alive()) {
        player_->update();
      } else {
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
    draw_game_objects();
  }

  draw_ui();

  EndDrawing();
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

  for (auto &b : bullets_) {
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
  const int center_x = SCREEN_WIDTH / 2;
  const int center_y = SCREEN_HEIGHT / 2;

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

      const char* stats_format = TextUtils::get_text("survival_stats");
      const char* final_stats = TextFormat(stats_format, game_time_, kill_count_);
      const int stats_width = MeasureText(final_stats, 20);
      DrawText(final_stats, center_x - stats_width/2, center_y + 80, 20, LIGHTGRAY);

      TextUtils::draw_text_centered("language_switch", center_x, center_y + 110, 16, LIGHTGRAY);
      break;
    }

    case GameState::PLAYING: {
      if (game_time_ < 10.0f) {
        TextUtils::draw_text_localized("move_controls",
                                     10, SCREEN_HEIGHT - 80, 16, LIGHTGRAY);
        TextUtils::draw_text_localized("auto_shoot_hint",
                                     10, SCREEN_HEIGHT - 60, 16, LIGHTGRAY);
        TextUtils::draw_text_localized("language_switch",
                                     10, SCREEN_HEIGHT - 40, 16, LIGHTGRAY);
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

  std::uniform_int_distribution<int> side_dist(0, 3);
  constexpr float offset = 100.f;

  switch (side_dist(gen)) {
    case 0: {
      // Зверху
      std::uniform_real_distribution<float> x_dist(0.0f, SCREEN_WIDTH);
      return Vector2{x_dist(gen), -offset};
    }
    case 1: {
      // Справа
      std::uniform_real_distribution<float> y_dist(0.0f, SCREEN_HEIGHT);
      return Vector2{SCREEN_WIDTH + offset, y_dist(gen)};
    }
    case 2: {
      // Знизу
      std::uniform_real_distribution<float> x_dist(0.0f, SCREEN_WIDTH);
      return Vector2{x_dist(gen), SCREEN_HEIGHT + offset};
    }
    case 3: {
      // Зліва
      std::uniform_real_distribution<float> y_dist(0.0f, SCREEN_HEIGHT);
      return Vector2{-offset, y_dist(gen)};
    }
    default:
      return Vector2{0, -offset};
  }
}

void Game::spawn_enemy() {
  if (enemies_.size() >= 50) return;

  auto spawn_pos = get_random_spawn_position();
  auto new_enemy = std::make_unique<ColoradoBeetle>(spawn_pos);

  if (new_enemy) {
    enemies_.push_back(std::move(new_enemy));
  }
}

void Game::spawn_bullet() {
  if (!player_ || !player_->is_alive()) return;

  auto* target = find_nearest_enemy();
  if (!target) return;

  auto bullet = std::make_unique<Bullet>(player_->get_position(), target->get_position(), 300.f, 25.f);

  bullets_.push_back(std::move(bullet));
  shoot_timer_ = shoot_interval_;
}

Enemy* Game::find_nearest_enemy() const {
  if (enemies_.empty() || !player_) return nullptr;

  const auto player_pos = player_->get_position();
  float min_distance = std::numeric_limits<float>::max();
  Enemy* nearest = nullptr;

  for (auto &e: enemies_) {
    if (!e || !e->is_alive()) continue;

    const auto [x, y] = e->get_position();
    const float dx = player_pos.x - x;
    const float dy = player_pos.y - y;

    if (const float distance = dx * dx + dy * dy; distance < min_distance) {
      min_distance = distance;
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

void Game::handle_input() {
  if (IsKeyPressed(KEY_P)) {
    state_ = (state_ == GameState::PLAYING) ? GameState::PAUSE : GameState::PLAYING;
  }

  if (IsKeyPressed(KEY_L)) {
    toggle_language();
  }

  if (IsKeyPressed(KEY_R) && state_ == GameState::GAMEOVER) {
    player_ = std::make_unique<Player>(Vector2{
      static_cast<float>(SCREEN_WIDTH) / 2.f,
      static_cast<float>(SCREEN_HEIGHT) / 2.f
    });
    enemies_.clear();
    bullets_.clear();
    spawn_timer_ = spawn_interval_;
    kill_count_ = 0;
    game_time_ = 0.0f;
    shoot_timer_ = 0.0f;
    state_ = GameState::PLAYING;
    spawn_interval_ = DEFAULT_SPAWN_INTERVAL;
    shoot_interval_ = DEFAULT_SHOOT_INTERVAL;

    state_ = GameState::PLAYING;
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
  Language new_lang = (current == Language::English) ?
                      Language::Ukrainian : Language::English;
  TextUtils::set_language(new_lang);
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
