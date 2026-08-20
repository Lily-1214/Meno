#ifndef GAMEOBJCECT_DEMO_COLLIDER_H
#define GAMEOBJCECT_DEMO_COLLIDER_H

#include "../../include/meno/math/Vec2.hpp"
#include "GameObject.h"

namespace meno {

enum class ColliderType { Box, Circle };

struct Collider {
    Vec2f offset{};
    bool isTrigger{false}; // 충돌 감지 여부, true이면 충돌 시 이벤트만 발생하고 물리적 반응은 없음
    int layer{0};

    Collider() = default;
    explicit Collider(const Vec2f& offset) : offset(offset) {}
    Collider(const Collider& other)
        : offset(other.offset), isTrigger(other.isTrigger), layer(other.layer) {}

    virtual ColliderType type() const noexcept = 0;
};

struct BoxCollider : public Collider {    
    Vec2f size{};

    BoxCollider() = default;
    BoxCollider(const Vec2f& offset, const Vec2f& size) : Collider(offset), size(size) {}

    ColliderType type() const noexcept override { return ColliderType::Box; }
};

struct CircleCollider : public Collider {
    float radius{};

    CircleCollider() = default;
    CircleCollider(const Vec2f& offset, float radius) : Collider (offset), radius(radius) {}

    ColliderType type() const noexcept override { return ColliderType::Circle; }
};

bool intersects(const GameObject& a, const GameObject& b);

} // namespace meno
#endif