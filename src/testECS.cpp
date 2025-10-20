#include "raylib.h"
#include "core/Entity.h"
#include "components/Transform.h"
#include "components/Sprite.h"
#include "systems/TransformSystem.h"
#include "systems/RenderSystem.h"
#include <iostream>
#include <vector>
#include <memory>

int main() {
    std::cout << "🎮 Starting ECS Test..." << std::endl;

    // Ініціалізація Raylib
    const int screenWidth = 800;
    const int screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "ECS Test - Transform + Render Systems");
    SetTargetFPS(60);

    // Створюємо системи
    TransformSystem transform_system;
    RenderSystem render_system;

    // Створюємо entities (використовуємо unique_ptr для автоматичного управління пам'яттю)
    std::vector<std::unique_ptr<Entity>> entities;

    // ❗ Гравець (синій, по центру)
    auto player = std::make_unique<Entity>();
    player->add_component<Components::Transform>(Vector2{screenWidth/2.0f, screenHeight/2.0f});
    player->add_component<Components::Sprite>(20.0f, BLUE);

    // ❗ Ворог 1 (червоний, зверху зліва)
    auto enemy1 = std::make_unique<Entity>();
    enemy1->add_component<Components::Transform>(Vector2{100, 100});
    enemy1->add_component<Components::Sprite>(15.0f, RED);

    // ❗ Ворог 2 (червоний, зверху справа)
    auto enemy2 = std::make_unique<Entity>();
    enemy2->add_component<Components::Transform>(Vector2{700, 100});
    enemy2->add_component<Components::Sprite>(15.0f, RED);

    // ❗ Ворог 3 (оранжевий, знизу зліва)
    auto enemy3 = std::make_unique<Entity>();
    enemy3->add_component<Components::Transform>(Vector2{100, 500});
    enemy3->add_component<Components::Sprite>(12.0f, ORANGE);

    // ❗ Фоновий об'єкт (зелений, великий, layer = -1)
    auto background = std::make_unique<Entity>();
    background->add_component<Components::Transform>(Vector2{screenWidth/2.0f, screenHeight/2.0f});
    auto* bg_sprite = background->add_component<Components::Sprite>(50.0f, DARKGREEN);
    bg_sprite->layer = -1;  // Малюється першим (фон)

    // Реєструємо entities в системах
    std::cout << "\n📝 Registering entities in systems..." << std::endl;

    transform_system.register_entity(player.get());
    transform_system.register_entity(enemy1.get());
    transform_system.register_entity(enemy2.get());
    transform_system.register_entity(enemy3.get());
    transform_system.register_entity(background.get());

    render_system.register_entity(player.get());
    render_system.register_entity(enemy1.get());
    render_system.register_entity(enemy2.get());
    render_system.register_entity(enemy3.get());
    render_system.register_entity(background.get());

    // Задаємо швидкості (px/sec)
    std::cout << "\n🏃 Setting velocities..." << std::endl;
    TransformSystem::set_velocity(player.get(), Vector2{100, 50});   // Вправо-вниз
    TransformSystem::set_velocity(enemy1.get(), Vector2{-50, 30});   // Вліво-вниз
    TransformSystem::set_velocity(enemy2.get(), Vector2{50, 40});    // Вправо-вниз
    TransformSystem::set_velocity(enemy3.get(), Vector2{-30, -50});  // Вліво-вгору
    // background залишається нерухомим (velocity = 0)

    // Межі світу
    Rectangle world_bounds = {0, 0,
                             static_cast<float>(screenWidth),
                             static_cast<float>(screenHeight)};

    std::cout << "\n✅ ECS Test initialized! Starting game loop...\n" << std::endl;

    // ❗ ГОЛОВНИЙ ІГРОВИЙ ЦИКЛ
    while (!WindowShouldClose()) {
        float delta_time = GetFrameTime();

        // ============================================
        // UPDATE PHASE (оновлення логіки)
        // ============================================

        // Оновлюємо всі transforms
        transform_system.update(delta_time);

        // Обмежуємо entities межами світу
        TransformSystem::clamp_to_world_bounds(player.get(), world_bounds);
        TransformSystem::clamp_to_world_bounds(enemy1.get(), world_bounds);
        TransformSystem::clamp_to_world_bounds(enemy2.get(), world_bounds);
        TransformSystem::clamp_to_world_bounds(enemy3.get(), world_bounds);

        // ============================================
        // RENDER PHASE (малювання)
        // ============================================

        BeginDrawing();
            ClearBackground(Color{34, 139, 34, 255}); // Зелена грядка

            // ❗ Рендерим всі entities (система сама сортує по layer)
            render_system.render();

            // UI інформація
            DrawText("🎮 ECS Test - Systems in Action!", 10, 10, 20, WHITE);
            DrawText(TextFormat("FPS: %d", GetFPS()), 10, 35, 20, WHITE);
            DrawText(TextFormat("Delta: %.4f", delta_time), 10, 60, 20, WHITE);

            // Позиції entities
            Vector2 player_pos = TransformSystem::get_position(player.get());
            DrawText(TextFormat("Player: (%.0f, %.0f)", player_pos.x, player_pos.y),
                    10, 90, 16, SKYBLUE);

            // Інструкції
            DrawText("All entities moving automatically!", 10, screenHeight - 60, 16, LIGHTGRAY);
            DrawText("They bounce off screen edges", 10, screenHeight - 40, 16, LIGHTGRAY);
            DrawText("ESC - Exit", 10, screenHeight - 20, 16, LIGHTGRAY);

        EndDrawing();
    }

    // Очищення
    std::cout << "\n🧹 Cleaning up..." << std::endl;
    transform_system.clear_entities();
    render_system.clear_entities();

    CloseWindow();

    std::cout << "👋 ECS Test completed!" << std::endl;
    return 0;
}
