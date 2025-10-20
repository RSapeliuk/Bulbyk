#pragma once
#include <string>
#include <unordered_map>
#include <vector>

#include "raylib.h"
#include "../components/Transform.h"
#include "../components/Sprite.h"

class Entity;

class RenderSystem {
private:
  std::vector<Entity*> entities_;

  std::unordered_map<std::string, Texture2D> textures_;

  Camera2D* camera_ = nullptr;

public:

  RenderSystem() = default;
  ~RenderSystem();

  void register_entity(Entity* entity);
  void unregister_entity(const Entity* entity);

  void load_texture(const std::string& name, const std::string& path);
  void unload_all_textures();

  void set_camera(Camera2D* camera) { camera_ = camera; }

  void render();

  void clear_entities();

private:

  bool is_valid_entity(const Entity* entity);

  void render_entity(const Entity* entity);
  void render_primitive(const Components::Transform *transform, const Components::Sprite* sprite);
  void render_sprite_texture(const Components::Transform *transform, const Components::Sprite *sprite);

  void sort_entities_by_layer();
};
