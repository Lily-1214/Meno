#include <meno/graphics/Renderer.hpp>

#include <meno/core/Window.hpp>
#include <meno/graphics/Font.hpp>
#include <meno/graphics/Texture.hpp>

#include "sfml_backend/Access.hpp"
#include "sfml_backend/Convert.hpp"

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/View.hpp>

#include <cmath>
#include <utility>

namespace meno {
namespace {

/// UTF-8 std::string_view를 SFML 문자열로. 한글이 깨지지 않게 하는 지점이다.
/// sf::String에 std::string을 그냥 넘기면 Latin-1로 해석해서 한글이 깨진다.
[[nodiscard]] sf::String toSfString(std::string_view text) {
    return sf::String::fromUtf8(text.begin(), text.end());
}

/// 도형 3종이 공유하는 변환 설정.
template <typename Shape>
void applyShapeParams(Shape& shape, const ShapeParams& params) {
    shape.setFillColor(backend::toSf(params.fill));
    shape.setOutlineColor(backend::toSf(params.outline));
    shape.setOutlineThickness(params.outlineThickness);
    shape.setRotation(backend::toSfAngle(params.rotation));
}

} // namespace

struct Renderer::Impl {
    /// 소유하지 않는다. Window가 Renderer보다 오래 산다는 것이 계약이다.
    sf::RenderWindow* target{nullptr};
};

Renderer::Renderer(Window& window) : impl_(std::make_unique<Impl>()) {
    impl_->target = backend::WindowAccess::native(window);
}

Renderer::~Renderer() = default;

Renderer::Renderer(Renderer&&) noexcept = default;

Renderer& Renderer::operator=(Renderer&&) noexcept = default;

// --- 프레임 ---------------------------------------------------------------

void Renderer::beginFrame(Color clearColor) {
    impl_->target->clear(backend::toSf(clearColor));
}

void Renderer::endFrame() {
    impl_->target->display();
}

// --- 스프라이트 -----------------------------------------------------------

void Renderer::draw(const Texture& texture, Vec2f position) {
    draw(texture, SpriteParams{.position = position});
}

void Renderer::draw(const Texture& texture, const SpriteParams& params) {
    const sf::Texture* native = backend::TextureAccess::native(texture);
    if (native == nullptr) {
        // 로드에 실패했거나 이동당한 텍스처. 조용히 건너뛴다.
        return;
    }

    // source가 없으면 텍스처 전체.
    const Vec2u textureSize = texture.size();
    Recti source = params.source.value_or(Recti{{0, 0},
                                               {static_cast<int>(textureSize.x),
                                                static_cast<int>(textureSize.y)}});

    // SFML은 텍스처 사각형의 폭/높이가 음수면 뒤집어서 그린다.
    // 스케일을 음수로 주는 방식과 달리 origin 계산이 틀어지지 않는다.
    if (params.flipX) {
        source.position.x += source.size.x;
        source.size.x = -source.size.x;
    }
    if (params.flipY) {
        source.position.y += source.size.y;
        source.size.y = -source.size.y;
    }

    sf::Sprite sprite{*native, backend::toSf(source)};
    sprite.setOrigin(backend::toSf(params.origin));
    sprite.setPosition(backend::toSf(params.position));
    sprite.setScale(backend::toSf(params.scale));
    sprite.setRotation(backend::toSfAngle(params.rotation));
    sprite.setColor(backend::toSf(params.tint));

    impl_->target->draw(sprite);
}

// --- 기본 도형 ------------------------------------------------------------

void Renderer::drawRect(const Rectf& rect, const ShapeParams& params) {
    sf::RectangleShape shape{backend::toSf(rect.size)};
    applyShapeParams(shape, params);
    shape.setOrigin(backend::toSf(params.origin));
    shape.setPosition(backend::toSf(rect.position));

    impl_->target->draw(shape);
}

void Renderer::drawCircle(Vec2f center, float radius, const ShapeParams& params) {
    sf::CircleShape shape{radius};
    applyShapeParams(shape, params);

    // CircleShape의 기준점은 바운딩 박스의 좌상단이다. origin을 반지름만큼
    // 옮겨야 인자로 받은 center가 실제 중심이 된다. params.origin은 그 위에
    // 얹히는 추가 오프셋으로 동작한다.
    shape.setOrigin(backend::toSf(Vec2f{radius, radius} + params.origin));
    shape.setPosition(backend::toSf(center));

    impl_->target->draw(shape);
}

void Renderer::drawLine(Vec2f from, Vec2f to, Color color, float thickness) {
    const Vec2f delta = to - from;
    const float length = delta.length();
    if (length <= 0.f) {
        return;
    }

    // 두께 있는 선은 회전시킨 사각형으로 그린다. VertexArray의 Lines는
    // 두께를 지원하지 않아서 thickness 인자가 무시되기 때문이다.
    sf::RectangleShape shape{sf::Vector2f{length, thickness}};
    shape.setFillColor(backend::toSf(color));
    shape.setOrigin(sf::Vector2f{0.f, thickness / 2.f});
    shape.setPosition(backend::toSf(from));
    shape.setRotation(sf::radians(std::atan2(delta.y, delta.x)));

    impl_->target->draw(shape);
}

// --- 텍스트 ---------------------------------------------------------------

void Renderer::drawText(const Font& font, std::string_view text, Vec2f position,
                        const TextParams& params) {
    const sf::Font* native = backend::FontAccess::native(font);
    if (native == nullptr) {
        return;
    }

    sf::Text drawable{*native, toSfString(text), params.characterSize};
    drawable.setFillColor(backend::toSf(params.color));
    drawable.setOutlineColor(backend::toSf(params.outlineColor));
    drawable.setOutlineThickness(params.outlineThickness);
    drawable.setOrigin(backend::toSf(params.origin));
    drawable.setPosition(backend::toSf(position));
    drawable.setRotation(backend::toSfAngle(params.rotation));

    impl_->target->draw(drawable);
}

Vec2f Renderer::measureText(const Font& font, std::string_view text,
                            const TextParams& params) const {
    const sf::Font* native = backend::FontAccess::native(font);
    if (native == nullptr) {
        return {};
    }

    const sf::Text drawable{*native, toSfString(text), params.characterSize};
    return backend::fromSf(drawable.getLocalBounds().size);
}

// --- 카메라 ---------------------------------------------------------------

void Renderer::setCamera(const Camera2D& camera) {
    // size가 {0,0}이면 프레임버퍼 크기를 써서 1픽셀 = 1월드유닛이 된다.
    Vec2f viewSize = camera.size;
    if (viewSize.x <= 0.f || viewSize.y <= 0.f) {
        viewSize = static_cast<Vec2f>(framebufferSize());
    }

    const float zoom = camera.zoom > 0.f ? camera.zoom : 1.f;
    viewSize /= zoom;

    sf::View view{backend::toSf(camera.center), backend::toSf(viewSize)};
    view.setRotation(backend::toSfAngle(camera.rotation));

    impl_->target->setView(view);
}

void Renderer::resetCamera() {
    impl_->target->setView(impl_->target->getDefaultView());
}

Vec2f Renderer::worldToScreen(Vec2f worldPoint) const {
    const sf::Vector2i pixel = impl_->target->mapCoordsToPixel(backend::toSf(worldPoint));
    return Vec2f{static_cast<float>(pixel.x), static_cast<float>(pixel.y)};
}

Vec2f Renderer::screenToWorld(Vec2f screenPoint) const {
    const sf::Vector2i pixel{static_cast<int>(screenPoint.x), static_cast<int>(screenPoint.y)};
    return backend::fromSf(impl_->target->mapPixelToCoords(pixel));
}

Rectf Renderer::visibleWorldBounds() const {
    const sf::View& view = impl_->target->getView();
    const Vec2f size = backend::fromSf(view.getSize());
    const Vec2f center = backend::fromSf(view.getCenter());
    return Rectf{center - size / 2.f, size};
}

// --- 정보 -----------------------------------------------------------------

Vec2u Renderer::framebufferSize() const {
    return backend::fromSf(impl_->target->getSize());
}

} // namespace meno
