#include "raylib.h"
#include "core/EntityManager.h"
#include "components/Transform.h"
#include "components/Sprite.h"
#include "systems/TransformSystem.h"
#include "systems/RenderSystem.h"
#include <iostream>

int main() {
    std::cout << "🎮 Starting ECS Test with EntityManager..." << std::endl;

    // Ініціалізація Raylib
    constexpr int screenWidth = 800;
    constexpr int screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "ECS Test - With EntityManager");
    SetTargetFPS(60);

    // ❗ НОВИЙ ПІДХІД: EntityManager керує всіма entities
    EntityManager entity_manager;

    // Створюємо системи
    TransformSystem transform_system;
    RenderSystem render_system;

    // ❗ Створюємо entities через manager (без unique_ptr!)
    Entity* player = entity_manager.create_entity();
    player->add_component<Components::Transform>(Vector2{screenWidth/2.0f, screenHeight/2.0f});
    player->add_component<Components::Sprite>(20.0f, BLUE);

    Entity* enemy1 = entity_manager.create_entity();
    enemy1->add_component<Components::Transform>(Vector2{100, 100});
    enemy1->add_component<Components::Sprite>(15.0f, RED);

    Entity* enemy2 = entity_manager.create_entity();
    enemy2->add_component<Components::Transform>(Vector2{700, 100});
    enemy2->add_component<Components::Sprite>(15.0f, RED);

    Entity* enemy3 = entity_manager.create_entity();
    enemy3->add_component<Components::Transform>(Vector2{100, 500});
    enemy3->add_component<Components::Sprite>(12.0f, ORANGE);

    Entity* background = entity_manager.create_entity();
    background->add_component<Components::Transform>(Vector2{screenWidth/2.0f, screenHeight/2.0f});
    auto* bg_sprite = background->add_component<Components::Sprite>(50.0f, DARKGREEN);
    bg_sprite->layer = -1;

    // ❗ АВТОМАТИЧНА РЕЄСТРАЦІЯ: отримуємо entities з потрібними компонентами
    std::cout << "\n🔄 Auto-registering entities in systems..." << std::endl;

    // Для TransformSystem потрібні entities з Transform
    const auto entities_with_transform = entity_manager.get_entities_with<Components::Transform>();
    for (Entity* entity : entities_with_transform) {
        transform_system.register_entity(entity);
    }

    // Для RenderSystem потрібні entities з Transform + Sprite
    const auto entities_with_render = entity_manager.get_entities_with<Components::Transform, Components::Sprite>();
    for (Entity* entity : entities_with_render) {
        render_system.register_entity(entity);
    }

    std::cout << "  Registered " << entities_with_transform.size()
              << " entities in TransformSystem" << std::endl;
    std::cout << "  Registered " << entities_with_render.size()
              << " entities in RenderSystem" << std::endl;

    // Задаємо швидкості
    TransformSystem::set_velocity(player, Vector2{100, 50});
    TransformSystem::set_velocity(enemy1, Vector2{-50, 30});
    TransformSystem::set_velocity(enemy2, Vector2{50, 40});
    TransformSystem::set_velocity(enemy3, Vector2{-30, -50});

    Rectangle world_bounds = {0, 0,
                             static_cast<float>(screenWidth),
                             static_cast<float>(screenHeight)};

    std::cout << "\n✅ ECS Test initialized!\n" << std::endl;

    // ❗ Змінні для тесту знищення entity
    float destroy_timer = 5.0f;  // Через 5 секунд знищимо enemy1
    bool enemy1_destroyed = false;

    // ❗ ГОЛОВНИЙ ІГРОВИЙ ЦИКЛ
    while (!WindowShouldClose()) {
        float delta_time = GetFrameTime();

        // ============================================
        // INPUT PHASE
        // ============================================

        // Тест знищення entity
        if (!enemy1_destroyed) {
            destroy_timer -= delta_time;
            if (destroy_timer <= 0.0f) {
                std::cout << "\n⏰ 5 seconds passed! Destroying enemy1..." << std::endl;

                // Unregister з систем
                transform_system.unregister_entity(enemy1);
                render_system.unregister_entity(enemy1);

                // Знищуємо через manager
                entity_manager.destroy_entity(enemy1);
                enemy1_destroyed = true;
            }
        }

        // ============================================
        // UPDATE PHASE
        // ============================================

        transform_system.update(delta_time);

        // Обмежуємо межами (тільки живі entities)
        for (Entity const* entity : entity_manager.get_entities_with<Components::Transform>()) {
            TransformSystem::clamp_to_world_bounds(entity, world_bounds);
        }

        // ❗ ОЧИЩЕННЯ: видаляємо знищені entities в кінці фрейму
        entity_manager.cleanup_destroyed_entities();

        // ============================================
        // RENDER PHASE
        // ============================================

        BeginDrawing();
            ClearBackground(Color{34, 139, 34, 255});

            render_system.render();

            // UI
            DrawText("🎮 ECS Test with EntityManager", 10, 10, 20, WHITE);
            DrawText(TextFormat("FPS: %d", GetFPS()), 10, 35, 20, WHITE);
            DrawText(TextFormat("Entities: %zu", entity_manager.get_entity_count()), 10, 60, 20, WHITE);

            // Таймер знищення
            if (!enemy1_destroyed) {
                DrawText(TextFormat("Enemy1 will be destroyed in: %.1fs", destroy_timer),
                        10, 90, 16, YELLOW);
            } else {
                DrawText("Enemy1 destroyed! (red circle disappeared)", 10, 90, 16, RED);
            }

            Vector2 player_pos = TransformSystem::get_position(player);
            DrawText(TextFormat("Player: (%.0f, %.0f)", player_pos.x, player_pos.y),
                    10, 120, 16, SKYBLUE);

            // Інструкції
            DrawText("Entities move and bounce automatically", 10, screenHeight - 60, 16, LIGHTGRAY);
            DrawText("Watch as enemy1 disappears after 5 seconds!", 10, screenHeight - 40, 16, LIGHTGRAY);
            DrawText("ESC - Exit", 10, screenHeight - 20, 16, LIGHTGRAY);

        EndDrawing();
    }

    // Очищення
    std::cout << "\n🧹 Shutting down..." << std::endl;
    transform_system.clear_entities();
    render_system.clear_entities();
    entity_manager.clear();

    CloseWindow();

    std::cout << "👋 ECS Test completed!" << std::endl;
    return 0;
}
