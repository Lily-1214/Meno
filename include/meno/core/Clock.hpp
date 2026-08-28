#pragma once

#include <chrono>
#include <functional>

namespace meno {

/// 실제 시간을 측정하는 고정밀도 시계.
/// NowFunction을 주입하면 대기 없이 단위 테스트할 수 있다.
class Clock {
public:
    using NativeClock = std::chrono::steady_clock;
    using TimePoint = NativeClock::time_point;
    using NowFunction = std::function<TimePoint()>;

    Clock();
    explicit Clock(NowFunction now);

    [[nodiscard]] double restart();
    [[nodiscard]] double elapsed() const;
    /// 0은 제한 없음. Window가 아닌 시간 계층이 프레임 제한을 담당한다.
    void setFramerateLimit(unsigned int framesPerSecond) noexcept;

private:
    NowFunction now_;
    TimePoint startTime_;
    TimePoint lastTime_;
    NativeClock::duration minimumFrameDuration_{};
};

} // namespace meno
