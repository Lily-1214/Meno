//
// Created by 최상준 on 26. 8. 6..
//

#ifndef GAMEOBJCECT_DEMO_GAMEOBJECT_H
#define GAMEOBJCECT_DEMO_GAMEOBJECT_H

#include "Vec2D.h"

struct Transform {
	Vec2D pos;              // 2D 위치 (x, y), 좌상단부터 (0, 0)
    int magnitude;          // 크기 배율
    int rotation;           // 360도 기준 회전
    int width, height;      // 크기, 너비
};

struct Collider {
	Vec2D offset;              // 2D 위치 (x, y), 좌상단부터 (0, 0)
	int width, height;      // 크기, 너비
};

// 렌더링 파이프라인 참고해서 수정 필요
struct Sprite {
	int width, height;      // 크기, 너비
	int* pixels;            // 픽셀 데이터
};

class GameObject {
private:
    Transform transform_;
    Collider collider_;
	Sprite sprite_;

public:
    GameObject();
    GameObject(const GameObject& other);
    GameObject(GameObject&& other) noexcept;

    Transform& transform() noexcept { return transform_; }
    Collider& collider() noexcept { return collider_; }
	Sprite& sprite() noexcept { return sprite_; }
};

#endif //GAMEOBJCECT_DEMO_GAMEOBJECT_H