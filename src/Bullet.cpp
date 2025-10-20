#include "Bullet.h"

#include <cmath>

#include "Constants.h"

Bullet::Bullet(Vector2 start_pos, Vector2 target_pos, float speed, float damage)
    : position_{start_pos}
    , speed_{speed}
    , damage_{damage}
    , radius_{3.0f}
    , active_{true}
    , color_{YELLOW}
    , lifetime_{0.0f}
    , max_lifetime_{5.0f}  // Куля живе 5 секунд максимум
{
    velocity_ = calculate_direction(start_pos, target_pos);
}

void Bullet::update() {
    if (!active_) return;

    const auto delta_time = GetFrameTime();

    // Оновлюємо позицію
    position_.x += velocity_.x * speed_ * delta_time;
    position_.y += velocity_.y * speed_ * delta_time;

    // Оновлюємо час життя
    lifetime_ += delta_time;
    if (lifetime_ >= max_lifetime_) {
        active_ = false;
        return;
    }

    // Перевіряємо межі екрану
    check_world_bounds();
}

void Bullet::draw() const {
    if (!active_) return;

    // Малюємо кулю
    DrawCircleV(position_, radius_, color_);

    // Додаємо ефект сяйва
    DrawCircleV(position_, radius_ * 0.6f, WHITE);

    // Додаємо trail effect (слід за кулею)
    const Vector2 trail_pos = {
        position_.x - velocity_.x * 0.1f,
        position_.y - velocity_.y * 0.1f
    };
    DrawCircleV(trail_pos, radius_ * 0.4f, ColorAlpha(color_, 0.5f));
}

Rectangle Bullet::get_bounds() const noexcept {
    return Rectangle{
        position_.x - radius_,
        position_.y - radius_,
        radius_ * 2.0f,
        radius_ * 2.0f
    };
}

void Bullet::check_world_bounds() {
    constexpr float margin = 100.0f;
    if (position_.x < -margin || position_.x > GameConstants::WORLD_WIDTH + margin ||
        position_.y < -margin || position_.y > GameConstants::WORLD_HEIGHT + margin) {
        active_ = false;
    }
}

Vector2 Bullet::calculate_direction(Vector2 start, Vector2 target) {
    Vector2 direction = {
        target.x - start.x,
        target.y - start.y
    };

    // Нормалізуємо вектор
    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);

    if (length > 0.0f) {
        direction.x /= length;
        direction.y /= length;
    } else {
        // Якщо довжина 0, стріляємо вправо
        direction = {1.0f, 0.0f};
    }

    return direction;
}


