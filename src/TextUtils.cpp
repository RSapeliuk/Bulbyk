#include "TextUtils.h"
#include <iostream>

// Ініціалізація статичних членів
std::unordered_map<std::string, std::unordered_map<Language, std::string>> TextUtils::translations_;
Language TextUtils::current_language_ = Language::English;

void TextUtils::init_translations() {
    std::cout << "Initializing localization system..." << std::endl;

    // UI Elements
    translations_["health"][Language::English] = "HP";
    translations_["health"][Language::Ukrainian] = "ZD"; // Здоров'я (скорочено для ASCII)

    translations_["killed"][Language::English] = "Killed";
    translations_["killed"][Language::Ukrainian] = "Vbyto";

    translations_["time"][Language::English] = "Time";
    translations_["time"][Language::Ukrainian] = "Chas";

    translations_["enemies"][Language::English] = "Enemies";
    translations_["enemies"][Language::Ukrainian] = "Vorogiv";

    translations_["bullets"][Language::English] = "Bullets";
    translations_["bullets"][Language::Ukrainian] = "Kul";

    translations_["seconds"][Language::English] = "sec";
    translations_["seconds"][Language::Ukrainian] = "sek";

    // Game States
    translations_["paused"][Language::English] = "PAUSED";
    translations_["paused"][Language::Ukrainian] = "PAUZA";

    translations_["game_over"][Language::English] = "GAME OVER";
    translations_["game_over"][Language::Ukrainian] = "KINEC GRY";

    translations_["game_title"][Language::English] = "BULBYK: TO THE ROOT OF EVIL";
    translations_["game_title"][Language::Ukrainian] = "BULBYK: DO KORENYA ZLA";

    // Controls
    translations_["restart_hint"][Language::English] = "R - restart";
    translations_["restart_hint"][Language::Ukrainian] = "R - restart";

    translations_["continue_hint"][Language::English] = "ESC - continue";
    translations_["continue_hint"][Language::Ukrainian] = "ESC - prodovzhyty";

    translations_["move_controls"][Language::English] = "WASD - move, ESC - pause";
    translations_["move_controls"][Language::Ukrainian] = "WASD - rukh, ESC - pauza";

    translations_["auto_shoot_hint"][Language::English] = "Auto-shoot at nearest enemy";
    translations_["auto_shoot_hint"][Language::Ukrainian] = "Avtostrilba v nayblyzhchoho voroga";

    translations_["language_switch"][Language::English] = "L - switch language";
    translations_["language_switch"][Language::Ukrainian] = "L - zminyty movu";

    // Game Messages
    translations_["game_start"][Language::English] = "Bulbyk begins his journey!";
    translations_["game_start"][Language::Ukrainian] = "Bulbyk pochynaye svoyu podorozh!";

    translations_["enemy_spawn"][Language::English] = "New enemy appeared! Total";
    translations_["enemy_spawn"][Language::Ukrainian] = "Novyy vorog z'yavyvsya! Vsogo";

    translations_["hit_target"][Language::English] = "Hit!";
    translations_["hit_target"][Language::Ukrainian] = "Vluchannya!";

    translations_["enemy_killed"][Language::English] = "Enemy destroyed! Total";
    translations_["enemy_killed"][Language::Ukrainian] = "Vorog znyshchenyy! Vsogo";

    translations_["player_damaged"][Language::English] = "Bulbyk took damage!";
    translations_["player_damaged"][Language::Ukrainian] = "Bulbyk otrymav poshkodzhennya!";

    translations_["game_restart"][Language::English] = "Game restarted!";
    translations_["game_restart"][Language::Ukrainian] = "Gra perezapushchena!";

    translations_["difficulty_increase"][Language::English] = "Difficulty level";
    translations_["difficulty_increase"][Language::Ukrainian] = "Riven skladnosti";

    // Stats Format Strings (для sprintf-like форматування)
    translations_["survival_stats"][Language::English] = "You survived %.1f seconds and killed %d enemies";
    translations_["survival_stats"][Language::Ukrainian] = "Ty protrymalsya %.1f sekund i vbyv %d vorogiv";

    translations_["spawn_rate"][Language::English] = "spawn rate";
    translations_["spawn_rate"][Language::Ukrainian] = "shvydkist spawn";

    // Debug Info
    translations_["debug_info"][Language::English] = "=== DEBUG INFO ===";
    translations_["debug_info"][Language::Ukrainian] = "=== DEBUG INFO ===";

    translations_["fps"][Language::English] = "FPS";
    translations_["fps"][Language::Ukrainian] = "FPS";

    translations_["delta"][Language::English] = "Delta";
    translations_["delta"][Language::Ukrainian] = "Delta";

    translations_["spawn_timer"][Language::English] = "Spawn Timer";
    translations_["spawn_timer"][Language::Ukrainian] = "Spawn Timer";

    translations_["shoot_timer"][Language::English] = "Shoot Timer";
    translations_["shoot_timer"][Language::Ukrainian] = "Shoot Timer";

    translations_["spawn_interval"][Language::English] = "Spawn Interval";
    translations_["spawn_interval"][Language::Ukrainian] = "Spawn Interval";

    translations_["max_enemies"][Language::English] = "Max Enemies";
    translations_["max_enemies"][Language::Ukrainian] = "Max Enemies";

    translations_["player_pos"][Language::English] = "Player";
    translations_["player_pos"][Language::Ukrainian] = "Player";

    std::cout << "Localization initialized! Language: "
              << (current_language_ == Language::English ? "English" : "Ukrainian")
              << std::endl;
}

void TextUtils::set_language(Language lang) {
    current_language_ = lang;
    std::cout << "Language changed to: "
              << (lang == Language::English ? "English" : "Ukrainian")
              << std::endl;
}

Language TextUtils::get_current_language() {
    return current_language_;
}

const char* TextUtils::get_text(const std::string& key) {
    auto key_it = translations_.find(key);
    if (key_it == translations_.end()) {
        std::cerr << "Warning: Translation key '" << key << "' not found!" << std::endl;
        return key.c_str(); // Повертаємо сам ключ як fallback
    }

    auto lang_it = key_it->second.find(current_language_);
    if (lang_it == key_it->second.end()) {
        // Якщо немає перекладу для поточної мови, пробуємо англійську
        auto en_it = key_it->second.find(Language::English);
        if (en_it != key_it->second.end()) {
            return en_it->second.c_str();
        }

        std::cerr << "Warning: No translation for key '" << key
                  << "' in current language!" << std::endl;
        return key.c_str();
    }

    return lang_it->second.c_str();
}

std::string TextUtils::get_formatted_text(const std::string& key, const std::string& format_args) {
    // Для складного форматування можна використовувати std::format (C++20)
    // Поки що повертаємо базовий текст
    return std::string(get_text(key));
}

void TextUtils::draw_text_localized(const std::string& key, int x, int y, int size, Color color) {
    DrawText(get_text(key), x, y, size, color);
}

void TextUtils::draw_text_centered(const std::string& key, int center_x, int y, int size, Color color) {
    const char* text = get_text(key);
    int text_width = MeasureText(text, size);
    DrawText(text, center_x - text_width/2, y, size, color);
}

int TextUtils::get_text_width(const std::string& key, int font_size) {
    return MeasureText(get_text(key), font_size);
}

bool TextUtils::has_translation(const std::string& key) {
    return translations_.find(key) != translations_.end();
}

void TextUtils::add_translation(const std::string& key, Language lang, const std::string& text) {
    translations_[key][lang] = text;
}
