#include <meno/core/Time.hpp>

namespace meno {

double Time::deltaTime_ = 0.0;
double Time::fixedDeltaTime_ = 1.0 / 60.0;
double Time::elapsedTime_ = 0.0;

double Time::deltaTime() noexcept { return deltaTime_; }
double Time::fixedDeltaTime() noexcept { return fixedDeltaTime_; }
double Time::elapsedTime() noexcept { return elapsedTime_; }

void Time::beginRun(double fixedDeltaTime) noexcept {
    deltaTime_ = 0.0;
    fixedDeltaTime_ = fixedDeltaTime;
    elapsedTime_ = 0.0;
}

void Time::advance(double deltaTime) noexcept {
    deltaTime_ = deltaTime;
    elapsedTime_ += deltaTime;
}

} // namespace meno
