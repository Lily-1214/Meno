#pragma once

namespace meno {

class Application;

/// Application이 관리하는 게임 시간 정보.
class Time final {
public:
    [[nodiscard]] static double deltaTime() noexcept;
    [[nodiscard]] static double fixedDeltaTime() noexcept;
    [[nodiscard]] static double elapsedTime() noexcept;

private:
    friend class Application;

    static void beginRun(double fixedDeltaTime) noexcept;
    static void advance(double deltaTime) noexcept;

    static double deltaTime_;
    static double fixedDeltaTime_;
    static double elapsedTime_;
};

} // namespace meno
