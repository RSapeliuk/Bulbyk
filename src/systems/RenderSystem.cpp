//
// Created by helpe on 20.10.2025.
//

#include "RenderSystem.h"

#include <algorithm>
#include <iostream>
#include <ranges>

#include "utils.h"

RenderSystem::~RenderSystem() {
  unload_all_textures();
}

void RenderSystem::register_entity(Entity *entity) {
  if (!is_valid_entity(entity)) {
    TRACELOG(LOG_WARNING, "Entity %d doesn't have required components (Transform + Sprite)!", entity->get_id());
    return;
  }

  if (const auto it = std::ranges::find(entities_, entity); it != entities_.end()) {
    TRACELOG(LOG_WARNING, "Entity %d already registered in RenderSystem!", entity->get_id());
    return;
  }

  entities_.push_back(entity);
  TRACELOG(LOG_INFO, "Entity %d registered in RenderSystem!", entity->get_id());
}

void RenderSystem::unregister_entity(const Entity *entity) {
  if (const auto it = std::ranges::find(entities_, entity); it != entities_.end()) {
    entities_.erase(it);
    TRACELOG(LOG_INFO, "Entity %d unregistered from RenderSystem!", entity->get_id());
  }
}

void RenderSystem::load_texture(const std::string &name, const std::string &path) {
  if (textures_.contains(name)) {
    TRACELOG(LOG_WARNING, "Texture %s already loaded!", name.c_str());
    return;
  }

  const Texture2D texture = LoadTexture(path.c_str());

  if (texture.id == 0) {
    TRACELOG(LOG_ERROR, "Failed to load texture %s", path.c_str());
    return;
  }

  textures_[name] = texture;
  TRACELOG(LOG_INFO, "Loaded texture %s (%s x %s), ", name.c_str(), texture.width, texture.height);
}

void RenderSystem::unload_all_textures() {
  for (const auto &texture: textures_ | std::views::values) {
    UnloadTexture(texture);
    TRACELOG(LOG_INFO, "Unloaded texture %s", texture.name.c_str());
  }
  textures_.clear();
}

void RenderSystem::render() {
  sort_entities_by_layer();

  if (camera_) {
    BeginMode2D(*camera_);
  }

  for (const Entity *entity: entities_) {
    if (is_valid_entity(entity)) {
      render_entity(entity);
    }
  }

  if (camera_) {
    EndMode2D();
  }
}

void RenderSystem::render_entity(const Entity *entity) {
  Components::Transform *transform = entity->get_component<Components::Transform>();
  Components::Sprite *sprite = entity->get_component<Components::Sprite>();

  if (!sprite->visible) {
    return;
  }

  if (sprite->use_primitive) {
    render_primitive(transform, sprite);
  } else {
    render_sprite_texture(transform, sprite);
  }
}

void RenderSystem::render_primitive(const Components::Transform *transform, const Components::Sprite *sprite) {
  const float radius = sprite->primitive_radius * transform->scale.x;

  DrawCircleV(transform->position, radius, sprite->primitive_color);

#ifdef _DEBUG
  DrawCircleLines(
    static_cast<int>(transform->position.x),
    static_cast<int>(transform->position.y),
    radius,
    RED);
#endif
}

void RenderSystem::render_sprite_texture(const Components::Transform *transform, const Components::Sprite *sprite) {
  const auto it = textures_.find(sprite->texture_name);
  if (it == textures_.end()) {
    TRACELOG(LOG_WARNING, "Failed to find texture %s. Fallback to PRIMITIVE rendering", sprite->texture_name.c_str());
    render_primitive(transform, sprite);
  }

  const Texture2D &texture = it->second;
  Rectangle source = sprite->source_rect;
  if (source.width == 0 || source.height == 0) {
    source = Rectangle{0, 0, static_cast<float>(texture.width), static_cast<float>(texture.height)};
  }

  const Rectangle dest = {
    transform->position.x,
    transform->position.y,
    source.width * transform->scale.x,
    source.height * transform->scale.y
  };

  Vector2 origin = {
    source.width * sprite->origin.x,
    source.height * sprite->origin.y
  };

  DrawTexturePro(texture, source, dest, origin, transform->rotation, sprite->tint);
}


void RenderSystem::sort_entities_by_layer() {
  std::sort(entities_.begin(), entities_.end(), [](Entity* a, Entity* b) {
    const auto sprite_a = a->get_component<Components::Sprite>();
    const auto sprite_b = b->get_component<Components::Sprite>();
    if (!sprite_a || !sprite_b) {
      return false;
    }
    return sprite_a->layer < sprite_b->layer;
  });
}

void RenderSystem::clear_entities() {
  entities_.clear();
  TRACELOG(LOG_INFO, "RenderSystem cleared all entities");
}

bool RenderSystem::is_valid_entity(const Entity *entity) {
  return entity && entity->has_component<Components::Transform>() && entity->has_component<Components::Sprite>();
}


