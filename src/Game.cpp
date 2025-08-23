#include "Game.h"

#include <algorithm>
#include <raymath.h>

#include "Player.h"
#include "Enemy.h"
#include "Bullet.h"

Game::Game(int screenWidth, int screenHeight, const char *title)
  : screenWidth(screenWidth)
    , screenHeight(screenHeight)
    , title(title)
    , state(GameState::PLAYING)
    , player(nullptr)
    , spawnTimer(0.0f)
    , spawnInterval(1.0f)
    , killCount(0) {
}

Game::~Game() {
  Unload();
  CloseAudioDevice();
  CloseWindow();
}

void Game::Init() {
  InitWindow(screenWidth, screenHeight, title);
  InitAudioDevice();
  player = new Player(screenWidth / 2.f, screenHeight / 2.f);

  std::srand((unsigned) time(NULL));
  spawnTimer = 0.0f;
  spawnInterval = 1.0f;
  killCount = 0;
}

void Game::Update() {
  const float dt = GetFrameTime();

  switch (state) {
    case GameState::PLAYING:
      spawnTimer += dt;
      if (spawnTimer >= spawnInterval) {
        spawnTimer -= spawnInterval;
        SpawnEnemy();
      }

      shootTimer += dt;
      if (shootTimer >= shootInterval) {
        shootTimer -= shootInterval;

        Vector2 pos = player->GetPosition();
        Vector2 target = GetMousePosition();
        Vector2 dir = Vector2Subtract(target, pos);
        float len = Vector2Length(dir);
        if (len != 0.0f) {
          dir.x /= len;
          dir.y /= len;
          Bullet *bullet = new Bullet(pos, dir, 500.f, 8.f);
          bullets.push_back(std::unique_ptr<Bullet>(bullet));
        }
      }

      player->Update();

      for (auto &e: enemies) {
        e->Update(player->GetPosition());
      }

      for (auto &b: bullets) {
        b->Update();
      }

      for (auto itE = enemies.begin(); itE != enemies.end();) {
        bool killed = false;
        for (auto itB = bullets.begin(); itB != bullets.end();) {
          if ((*itB)->GetBounds().x == (*itB)->GetBounds().x && CheckCollisionRecs(
                (*itB)->GetBounds(), (*itE)->GetBounds())) {
            killed = true;
            itB = bullets.erase(itB);
            killCount++;
            break;
          }
          ++itB;
        }
        if (killed) {
          itE = enemies.erase(itE);
          killCount++;
        } else {
          ++itE;
        }
      }

      if (killCount >= 10) {
        state = GameState::BOSS;
      }

      bullets.erase(std::remove_if(bullets.begin(), bullets.end(), [](const std::unique_ptr<Bullet> &b) {
        return !b->IsAlive();
      }), bullets.end());

      break;
    case GameState::BOSS:
      break;
    default:
      break;
  }
}

void Game::Draw() {
  BeginDrawing();
  ClearBackground(RAYWHITE);
  switch (state) {
    case GameState::PLAYING:
      player->Draw();
      for (auto &e: enemies) {
        e->Draw();
      }
      for (auto &b: bullets) {
        b->Draw();
      }
      break;
    case GameState::BOSS:
      break;
    default:
      break;
  }

  DrawText(TextFormat("Kills: %d", killCount), 10, 10, 20, BLACK);

  EndDrawing();
}

void Game::Run() {
  Init();
  while (!WindowShouldClose()) {
    Update();
    Draw();
  }
}

void Game::Unload() {
  if (player) {
    delete player;
    player = nullptr;
  }

  enemies.clear();
}

void Game::SpawnEnemy() {
  float size = 20.f;
  float speed = 100.f + GetRandomValue(0.f, 100);

  Vector2 pos;
  int side = GetRandomValue(0, 3);
  switch (side) {
    case 0:
      pos.x = (float) GetRandomValue(0, screenWidth);
      pos.y = -size;
      break;
    case 1:
      pos.x = (float) GetRandomValue(0, screenWidth);
      pos.y = screenHeight + size;
      break;
    case 2:
      pos.x = -size;
      pos.y = (float) GetRandomValue(0, screenHeight);
      break;
    case 3:
      pos.x = screenWidth + size;
      pos.y = (float) GetRandomValue(0, screenHeight);
      break;
  }

  enemies.push_back(std::make_unique<Enemy>(pos, speed, size));
}
