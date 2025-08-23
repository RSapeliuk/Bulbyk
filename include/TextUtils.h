#pragma once
#include "raylib.h"
#include <string>
#include <unordered_map>

enum class Language {
  English,
  Ukrainian,
  // Можна додавати інші мови в майбутньому
};

class TextUtils {
private:
  static std::unordered_map<std::string, std::unordered_map<Language, std::string>> translations_;
  static Language current_language_;

public:
  static void init_translations();
  static void set_language(Language lang);
  static Language get_current_language();

  // Основні методи для отримання тексту
  static const char* get_text(const std::string& key);
  static std::string get_formatted_text(const std::string& key, const std::string& format_args);

  // Методи для малювання
  static void draw_text_localized(const std::string& key, int x, int y, int size, Color color);
  static void draw_text_centered(const std::string& key, int center_x, int y, int size, Color color);
  static int get_text_width(const std::string& key, int font_size);

  // Утилітарні методи
  static bool has_translation(const std::string& key);
  static void add_translation(const std::string& key, Language lang, const std::string& text);
};
