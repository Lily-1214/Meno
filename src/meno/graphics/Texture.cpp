#include <meno/graphics/Texture.hpp>

#include "sfml_backend/Access.hpp"
#include "sfml_backend/Convert.hpp"

#include <SFML/Graphics/Image.hpp>

#include <utility>

namespace meno {

struct Texture::Impl {
    sf::Texture texture;
    bool loaded{false};
};

Texture::Texture() : impl_(std::make_unique<Impl>()) {}

Texture::~Texture() = default;

Texture::Texture(Texture&&) noexcept = default;

Texture& Texture::operator=(Texture&&) noexcept = default;

std::optional<Texture> Texture::loadFromFile(const std::filesystem::path& path) {
    Texture result;
    if (!result.impl_->texture.loadFromFile(path)) {
        return std::nullopt;
    }
    result.impl_->loaded = true;
    return result;
}

std::optional<Texture> Texture::fromPixels(Vec2u size, const std::uint8_t* rgba) {
    if (rgba == nullptr || size.x == 0 || size.y == 0) {
        return std::nullopt;
    }

    const sf::Image image{backend::toSf(size), rgba};

    Texture result;
    if (!result.impl_->texture.loadFromImage(image)) {
        return std::nullopt;
    }
    result.impl_->loaded = true;
    return result;
}

Vec2u Texture::size() const {
    return backend::fromSf(impl_->texture.getSize());
}

void Texture::setSmooth(bool smooth) {
    impl_->texture.setSmooth(smooth);
}

bool Texture::isSmooth() const {
    return impl_->texture.isSmooth();
}

namespace backend {

const sf::Texture* TextureAccess::native(const Texture& texture) noexcept {
    // 이동 후 빈 껍데기(impl_ == nullptr)일 수 있으므로 둘 다 확인한다.
    if (texture.impl_ == nullptr || !texture.impl_->loaded) {
        return nullptr;
    }
    return &texture.impl_->texture;
}

} // namespace backend

} // namespace meno
