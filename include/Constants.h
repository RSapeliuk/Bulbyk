#pragma once

namespace GameConstants {
    // ===========================================
    // WORLD & SCREEN DIMENSIONS
    // ===========================================
    constexpr int SCREEN_WIDTH = 1024;
    constexpr int SCREEN_HEIGHT = 768;
    constexpr int WORLD_WIDTH = 2048;
    constexpr int WORLD_HEIGHT = 1536;
    
    // ===========================================
    // GAME BALANCE
    // ===========================================
    namespace Gameplay {
        constexpr float DEFAULT_SPAWN_INTERVAL = 2.0f;
        constexpr float DEFAULT_SHOOT_INTERVAL = 1.7f;
        constexpr float DIFFICULTY_UPDATE_TIME = 5.0f;
        constexpr float DIFFICULTY_LEVEL_TIME = 30.0f;
        constexpr float MIN_SPAWN_INTERVAL = 0.5f;
        constexpr float MIN_SHOOT_INTERVAL = 0.05f;
        constexpr int BASE_MAX_ENEMIES = 20;
        constexpr int ENEMIES_PER_DIFFICULTY_LEVEL = 5;
        constexpr float DIFFICULTY_SPAWN_DECREASE = 0.2f;
        constexpr float DIFFICULTY_SHOOT_DECREASE = 0.02f;
    }
    
    // ===========================================
    // PLAYER SETTINGS  
    // ===========================================
    namespace Player {
        constexpr float SPEED = 200.0f;
        constexpr float RADIUS = 20.0f;
        constexpr int MAX_HEALTH = 100;
        constexpr Color COLOR = BROWN;
        constexpr float EYE_OFFSET = 8.0f;
        constexpr float EYE_Y_OFFSET = -5.0f;
        constexpr float EYE_RADIUS = 3.0f;
        constexpr float HEALTH_BAR_HEIGHT = 4.0f;
        constexpr float HEALTH_BAR_Y_OFFSET = -15.0f;
        constexpr float DIAGONAL_MULTIPLIER = 0.707106781f;  // 1/sqrt(2)
    }
    
    // ===========================================
    // BULLET SETTINGS
    // ===========================================
    namespace Bullet {
        constexpr float SPEED = 300.0f;
        constexpr float DAMAGE = 25.0f;
        constexpr float RADIUS = 3.0f;
        constexpr float MAX_LIFETIME = 5.0f;
        constexpr float WORLD_MARGIN = 100.0f;
        constexpr Color COLOR = YELLOW;
        constexpr float GLOW_RADIUS_MULTIPLIER = 0.6f;
        constexpr float TRAIL_RADIUS_MULTIPLIER = 0.4f;
        constexpr float TRAIL_OFFSET = 0.1f;
        constexpr float TRAIL_ALPHA = 0.5f;
    }
    
    // ===========================================
    // ENEMY SETTINGS
    // ===========================================
    namespace Enemy {
        constexpr float SPAWN_MARGIN = 100.0f;
        constexpr float HEALTH_BAR_HEIGHT = 3.0f;
        constexpr float HEALTH_BAR_Y_OFFSET = -10.0f;
        
        namespace ColoradoBeetle {
            constexpr float HEALTH = 30.0f;
            constexpr float SPEED = 50.0f;
            constexpr float DAMAGE = 10.0f;
            constexpr float RADIUS = 15.0f;
            constexpr float ATTACK_INTERVAL = 1.0f;
            constexpr Color COLOR = ORANGE;
            // Детали жука
            constexpr float STRIPE_OFFSET_X = 5.0f;
            constexpr float STRIPE_OFFSET_Y = 3.0f;
            constexpr float STRIPE_RADIUS = 2.0f;
            constexpr float BODY_OFFSET_Y = 5.0f;
            constexpr float BODY_RADIUS = 3.0f;
        }
        
        // Готовність для нових ворогів
        namespace Aphid {
            constexpr float HEALTH = 10.0f;
            constexpr float SPEED = 80.0f;
            constexpr float DAMAGE = 5.0f;
            constexpr float RADIUS = 8.0f;
            constexpr Color COLOR = GREEN;
        }
    }
    
    // ===========================================
    // CAMERA SETTINGS
    // ===========================================
    namespace Camera {
        constexpr float FOLLOW_SPEED = 5.0f;
        constexpr float MIN_ZOOM = 0.5f;
        constexpr float MAX_ZOOM = 3.0f;
        constexpr float DEFAULT_ZOOM = 1.0f;
        constexpr float DEFAULT_ROTATION = 0.0f;
    }
    
    // ===========================================
    // UI SETTINGS
    // ===========================================
    namespace UI {
        constexpr int MARGIN = 10;
        constexpr int FONT_SIZE = 20;
        constexpr int LINE_HEIGHT = 25;
        constexpr int SMALL_FONT_SIZE = 16;
        constexpr int LARGE_FONT_SIZE = 40;
        constexpr int HUGE_FONT_SIZE = 50;
        constexpr int MEDIUM_FONT_SIZE = 25;
        constexpr float INSTRUCTION_DISPLAY_TIME = 10.0f;
        
        namespace Minimap {
            constexpr int SIZE = 150;
            constexpr int MARGIN_FROM_EDGE = 10;
            constexpr float BACKGROUND_ALPHA = 0.7f;
            constexpr int PLAYER_DOT_RADIUS = 3;
            constexpr int ENEMY_DOT_RADIUS = 1;
        }
        
        namespace Colors {
            constexpr Color BACKGROUND_PLAYING = Color{34, 139, 34, 255};
            constexpr Color BACKGROUND_PAUSED = Color{64, 64, 64, 255};
            constexpr Color BACKGROUND_GAMEOVER = Color{139, 34, 34, 255};
            constexpr Color WORLD_BOUNDS = RED;
            constexpr Color DANGER_ZONE = DARKPURPLE;
            constexpr Color GRID_LINES = Color{0, 0, 0, 25};
            constexpr Color HEALTH_BAR_BACKGROUND = RED;
            constexpr Color HEALTH_BAR_FOREGROUND = GREEN;
            constexpr Color MINIMAP_BACKGROUND = Color{0, 0, 0, 179};  // ColorAlpha(BLACK, 0.7f)
            constexpr Color MINIMAP_BORDER = WHITE;
            constexpr Color MINIMAP_PLAYER = BLUE;
            constexpr Color MINIMAP_ENEMY = RED;
            constexpr Color MINIMAP_CAMERA = YELLOW;
        }
        
        namespace Text {
            constexpr Color PRIMARY = WHITE;
            constexpr Color SECONDARY = LIGHTGRAY;
            constexpr Color WARNING = YELLOW;
            constexpr Color ERROR = RED;
            constexpr Color SUCCESS = GREEN;
            constexpr Color DEBUG = ORANGE;
        }
    }
    
    // ===========================================
    // WORLD DESIGN
    // ===========================================
    namespace World {
        constexpr int GRID_SIZE = 64;
        constexpr int DANGER_ZONE_SIZE = 20;
        constexpr float DANGER_ZONE_LINE_WIDTH = 4.0f;
        constexpr float GRID_ALPHA = 0.1f;
    }
    
    // ===========================================
    // SPAWN SYSTEM
    // ===========================================
    namespace Spawn {
        constexpr int SPAWN_SIDES = 4;  // top, right, bottom, left
        constexpr float CAMERA_MARGIN = 100.0f;
        constexpr float MIN_SPAWN_DISTANCE = 50.0f;
    }
    
    // ===========================================
    // DEBUG SETTINGS
    // ===========================================
    namespace Debug {
        constexpr int INFO_PANEL_WIDTH = 250;
        constexpr int INFO_MARGIN = 10;
        constexpr int LINE_HEIGHT = 20;
        constexpr float UPDATE_INTERVAL = 1.0f;
        constexpr int FONT_SIZE = 16;
    }
    
    // ===========================================
    // PERFORMANCE SETTINGS
    // ===========================================
    namespace Performance {
        constexpr int TARGET_FPS = 60;
        constexpr int MAX_ENEMIES_ABSOLUTE = 200;  // Абсолютний ліміт для performance
        constexpr int MAX_BULLETS_ABSOLUTE = 500;
        constexpr float CLEANUP_INTERVAL = 0.1f;  // Як часто чистити мертві об'єкти
    }
}

// ===========================================
// UTILITY MACROS
// ===========================================
#define SCREEN_CENTER_X (GameConstants::SCREEN_WIDTH / 2.0f)
#define SCREEN_CENTER_Y (GameConstants::SCREEN_HEIGHT / 2.0f)
#define WORLD_CENTER_X (GameConstants::WORLD_WIDTH / 2.0f)
#define WORLD_CENTER_Y (GameConstants::WORLD_HEIGHT / 2.0f)

#define SCREEN_CENTER Vector2{SCREEN_CENTER_X, SCREEN_CENTER_Y}
#define WORLD_CENTER Vector2{WORLD_CENTER_X, WORLD_CENTER_Y}
