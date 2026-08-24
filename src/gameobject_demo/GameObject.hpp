//
// Created by 최상준 on 26. 8. 6..
//

#ifndef GAMEOBJCECT_DEMO_GAMEOBJECT_H
#define GAMEOBJCECT_DEMO_GAMEOBJECT_H

#include "../../include/meno/math/Vec2.hpp"
#include "collider.hpp"
#include "component.hpp"

#include <memory>
#include <unordered_map>
#include <typeindex>

namespace meno {

struct Transform : public Component {
    Vec2f pos;         // 2D 위치 (x, y), 좌상단부터 (0, 0)
    int magnitude;     // 크기 배율
    int rotation;      // 360도 기준 회전
    int width, height; // 크기, 너비
};

// 렌더링 파이프라인 참고해서 수정 필요
struct Sprite : public Component {
    int width, height; // 크기, 너비
    int* pixels;       // 픽셀 데이터
};

class GameObject {
private:
    Transform transform_;

    std::unordered_map<std::type_index, std::unique_ptr<Component>> components_;

public:
    GameObject() = default;

    GameObject(const GameObject&) = delete;
    GameObject& operator=(const GameObject&) = delete;

    GameObject(GameObject&&) noexcept = default;
    GameObject& operator=(GameObject&&) noexcept = default;

    Transform& transform() noexcept { return transform_; }

    template <typename T, typename... Args>
        requires std::derived_from<T, Component>
    T& addComponent(Args&&... args) {
        auto ptr = std::make_unique<T>(std::forward<Args>(args)...);

        T& ref = *ptr;

        components_[std::type_index(typeid(T))] = std::move(ptr);

        return ref;
    }

    template <typename T>
        requires std::derived_from<T, Component>
    T* getComponent() noexcept {
        auto it = components_.find(std::type_index(typeid(T)));

        if (it == components_.end())
            return nullptr;

        return static_cast<T*>(it->second.get());
    }
};

} // namespace meno
#endif //GAMEOBJCECT_DEMO_GAMEOBJECT_H