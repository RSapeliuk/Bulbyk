#pragma once
#include <cstdint>
#include <memory>
#include <typeindex>
#include <unordered_map>

using EntityID = std::uint32_t;
constexpr EntityID INVALID_ENTITY_ID = 0;

class Component {
public:
  virtual ~Component() = default;
};

class Entity {
private:
  static EntityID next_id_;
  EntityID id_;
  std::unordered_map<std::type_index, std::unique_ptr<Component>> components_;

public:
  Entity() : id_(next_id_++) {}

  EntityID get_id() const { return id_; }

  template<typename T, typename... Args>
  T* add_component(Args&&... args) {
    auto component = std::make_unique<T>(std::forward<Args>(args)...);
    T* ptr = component.get();
    components_[std::type_index(typeid(T))] = std::move(component);
    return ptr;
  }

  template<typename T>
  T* get_component() const {
    if (const auto it = components_.find(std::type_index(typeid(T))); it != components_.end()) {
      return static_cast<T*>(it->second.get());
    }

    return nullptr;
  }

  template<typename T>
  bool has_component() const {
    return components_.contains(std::type_index(typeid(T)));
  }
};
