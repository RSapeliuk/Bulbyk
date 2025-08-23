#pragma once

#include<GameState.h>
#include <memory>
#include <vector>


class Player;
class Enemy;
class Bullet;

class Game {
public:
  Game(int screenWidth, int screenHeight, const char *title);

  ~Game();

  void Run();

private:
  void Init();

  void Update();

  void Draw();

  void Unload();

  void SpawnEnemy();

  int screenWidth;
  int screenHeight;
  const char *title;
  GameState state;
  Player *player;

  std::vector<std::unique_ptr<Enemy>> enemies;
  float spawnTimer;
  float spawnInterval;
  int killCount;

  std::vector<std::unique_ptr<Bullet>> bullets;
  float shootTimer = 0.0f;
  float shootInterval = 0.2f;
};
