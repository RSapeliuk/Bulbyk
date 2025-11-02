//
// Created by helpe on 22.10.2025.
//

#include "raylib.h"
#include "core/EntityManager.h"
#include "components/Transform.h"
#include "components/Sprite.h"
#include "components/Collider.h"
#include "systems/TransformSystem.h"
#include "systems/RenderSystem.h"
#include "systems/CollisionSystem.h"
#include <iostream>

using namespace Components;

int main() {
    std::cout << "🎮 Testing CollisionSystem..." << std::endl;

    const int screenWidth = 800;
    const int screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "CollisionSystem Test");
    SetTargetFPS(60);

    // Створюємо менеджер і системи
    EntityManager entity_manager;
    TransformSystem transform_system;
    RenderSystem render_system;
    CollisionSystem collision_system;

    // ❗ Створюємо гравця
    Entity *player = entity_manager.create_entity();
    player->add_component<Components::Transform>(Vector2{400, 300});
    player->add_component<Sprite>(20.0f, BLUE);
    auto *player_collider = player->add_component<Collider>(
        20.0f, // radius
        CollisionLayer::PLAYER, // layer
        false // не trigger
    );
    player_collider->mask = CollisionLayer::ENEMY | CollisionLayer::OBSTACLE | CollisionLayer::PICKUP;
    player_collider->debug_color = SKYBLUE;

    // ❗ Створюємо ворогів
    Entity *enemy1 = entity_manager.create_entity();
    enemy1->add_component<Components::Transform>(Vector2{200, 200});
    enemy1->add_component<Sprite>(15.0f, RED);
    auto *enemy1_collider = enemy1->add_component<Collider>(
        15.0f,
        CollisionLayer::ENEMY,
        false
    );
    enemy1_collider->mask = CollisionLayer::PLAYER | CollisionLayer::BULLET;
    enemy1_collider->debug_color = PINK;

    Entity *enemy2 = entity_manager.create_entity();
    enemy2->add_component<Components::Transform>(Vector2{600, 400});
    enemy2->add_component<Sprite>(15.0f, RED);
    auto *enemy2_collider = enemy2->add_component<Collider>(
        15.0f,
        CollisionLayer::ENEMY,
        false
    );
    enemy2_collider->mask = CollisionLayer::PLAYER | CollisionLayer::BULLET;
    enemy2_collider->debug_color = PINK;

    // ❗ Створюємо trigger зону (pickup)
    Entity *pickup = entity_manager.create_entity();
    pickup->add_component<Components::Transform>(Vector2{400, 150});
    pickup->add_component<Sprite>(25.0f, GOLD);
    auto *pickup_collider = pickup->add_component<Collider>(
        25.0f,
        CollisionLayer::PICKUP,
        true // ❗ це trigger - не блокує рух
    );
    pickup_collider->type = ColliderType::CIRCLE;
    pickup_collider->mask = CollisionLayer::PLAYER;
    pickup_collider->debug_color = YELLOW;

    // ❗ Створюємо стіну (прямокутник)
    Entity *wall = entity_manager.create_entity();
    wall->add_component<Components::Transform>(Vector2{700, 300});
    wall->add_component<Sprite>(60.0f, GRAY);
    auto *wall_collider = wall->add_component<Collider>(
        Vector2{60, 100}, // size для Rectangle
        CollisionLayer::OBSTACLE,
        false
    );
    wall_collider->type = ColliderType::RECTANGLE;
    wall_collider->mask = CollisionLayer::PLAYER | CollisionLayer::ENEMY;
    wall_collider->debug_color = DARKGRAY;

    // Реєструємо в системах
    for (Entity *e: entity_manager.get_entities_with<Components::Transform>()) {
        transform_system.register_entity(e);
    }

    for (Entity *e: entity_manager.get_entities_with<Components::Transform, Sprite>()) {
        render_system.register_entity(e);
    }

    for (Entity *e: entity_manager.get_entities_with<Components::Transform, Collider>()) {
        collision_system.register_entity(e);
    }

    // ❗ Реєструємо collision callback
    int collision_count = 0;
    bool player_in_pickup = false;

    collision_system.add_collision_callback([&](const CollisionInfo &info) {
        collision_count++;

        // ❗ DEBUG: виводь ВСІ колізії
        std::cout << "Collision: Entity #" << info.entity_a->get_id()
                  << " <-> Entity #" << info.entity_b->get_id() << std::endl;

        // Перевіряємо trigger (pickup)
        Entity* player_entity = nullptr;
        Entity* pickup_entity = nullptr;

        // Визначаємо хто хто
        if (info.entity_a == player && info.entity_b == pickup) {
            player_entity = info.entity_a;
            pickup_entity = info.entity_b;
        } else if (info.entity_a == pickup && info.entity_b == player) {
            player_entity = info.entity_b;
            pickup_entity = info.entity_a;
        }

        // Якщо знайшли гравця і pickup
        if (player_entity && pickup_entity) {
            player_in_pickup = true;
            std::cout << "💰 Player in pickup zone!" << std::endl;
        }

        // Перевіряємо колізію з ворогом
        if ((info.entity_a == player || info.entity_b == player) &&
            (info.entity_a == enemy1 || info.entity_b == enemy1 ||
             info.entity_a == enemy2 || info.entity_b == enemy2)) {
            std::cout << "💥 Player hit enemy! Penetration: "
                      << info.penetration_depth << std::endl;
        }

        // Перевіряємо колізію зі стіною
        if ((info.entity_a == player || info.entity_b == player) &&
            (info.entity_a == wall || info.entity_b == wall)) {
            std::cout << "🧱 Player hit wall!" << std::endl;
        }
    });

    // Рух ворогів до гравця
    TransformSystem::set_velocity(enemy1, Vector2{50, 30});
    TransformSystem::set_velocity(enemy2, Vector2{-40, -20});

    std::cout << "\n✅ Test ready! Use WASD to move player\n" << std::endl;

    bool show_debug_colliders = true;

    // Головний цикл
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        // ============================================
        // INPUT
        // ============================================

        // Керування гравцем
        Vector2 input = {0, 0};
        if (IsKeyDown(KEY_A)) input.x = -1;
        if (IsKeyDown(KEY_D)) input.x = 1;
        if (IsKeyDown(KEY_W)) input.y = -1;
        if (IsKeyDown(KEY_S)) input.y = 1;

        // Нормалізація діагоналі
        if (input.x != 0 && input.y != 0) {
            input.x *= 0.707f;
            input.y *= 0.707f;
        }

        TransformSystem::set_velocity(player, Vector2{input.x * 200, input.y * 200});

        // Toggle debug
        if (IsKeyPressed(KEY_SPACE)) {
            show_debug_colliders = !show_debug_colliders;
        }

        // ============================================
        // UPDATE
        // ============================================

        collision_count = 0;
        player_in_pickup = false;

        transform_system.update(dt);

        // ❗ ПЕРЕВІРКА КОЛІЗІЙ
        collision_system.update();

        // Обмеження світу
        Rectangle world = {0, 0, 800, 600};
        for (Entity *e: entity_manager.get_entities_with<Components::Transform>()) {
            TransformSystem::clamp_to_world_bounds(e, world);
        }

        // ============================================
        // RENDER
        // ============================================

        BeginDrawing();
        ClearBackground(Color{34, 139, 34, 255});

        render_system.render();

        // ❗ Debug colliders
        if (show_debug_colliders) {
            collision_system.debug_draw();
        }

        // UI
        DrawText("🎮 CollisionSystem Test", 10, 10, 20, WHITE);
        DrawText("WASD - Move Player", 10, 35, 16, WHITE);
        DrawText("SPACE - Toggle Debug Colliders", 10, 55, 16, WHITE);

        DrawText(TextFormat("Collisions this frame: %d", collision_count), 10, 85, 16, YELLOW);

        if (player_in_pickup) {
            DrawText("💰 IN PICKUP ZONE!", 10, 110, 20, GOLD);
        }

        DrawText(TextFormat("FPS: %d", GetFPS()), 10, 140, 16, WHITE);

        // Легенда
        DrawText("Blue = Player", 10, screenHeight - 90, 14, SKYBLUE);
        DrawText("Red = Enemies", 10, screenHeight - 70, 14, PINK);
        DrawText("Gold = Pickup (trigger)", 10, screenHeight - 50, 14, YELLOW);
        DrawText("Gray = Wall", 10, screenHeight - 30, 14, DARKGRAY);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
