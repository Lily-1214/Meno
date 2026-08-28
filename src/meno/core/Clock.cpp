#include <meno/core/Clock.hpp>

#include <utility>
#include <thread>

namespace meno {

Clock::Clock() : Clock([] { return NativeClock::now(); }) {}

Clock::Clock(NowFunction now) : now_(std::move(now)) {
    startTime_ = now_();
    lastTime_ = startTime_;
}

double Clock::restart() {
    TimePoint current = now_();
    const TimePoint earliestFrameEnd = lastTime_ + minimumFrameDuration_;
    if (minimumFrameDuration_ > NativeClock::duration::zero() &&
        current < earliestFrameEnd) {
        std::this_thread::sleep_until(earliestFrameEnd);
        current = now_();
    }
    const std::chrono::duration<double> delta = current - lastTime_;
    lastTime_ = current;
    return delta.count();
}

void Clock::setFramerateLimit(unsigned int framesPerSecond) noexcept {
    if (framesPerSecond == 0) {
        minimumFrameDuration_ = NativeClock::duration::zero();
        return;
    }
    minimumFrameDuration_ = std::chrono::duration_cast<NativeClock::duration>(
        std::chrono::duration<double>(1.0 / static_cast<double>(framesPerSecond)));
}

double Clock::elapsed() const {
    return std::chrono::duration<double>(now_() - startTime_).count();
}

} // namespace meno
