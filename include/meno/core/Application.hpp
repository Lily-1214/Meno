#pragma once

#include <cstddef>

namespace meno {

class Clock;

struct ApplicationConfig {
    double fixedTimeStep = 1.0 / 60.0;
    double maxFrameTime = 0.25;
    std::size_t maxUpdatesPerFrame = 8;
};

/// 고정 timestep 게임 루프의 기본 클래스.
class Application {
public:
    explicit Application(ApplicationConfig config = {});
    virtual ~Application() = default;

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    void run();
    void run(Clock& clock);
    void stop() noexcept;
    [[nodiscard]] bool isRunning() const noexcept;

protected:
    virtual void onStart();
    virtual void processEvents();
    virtual void update(double fixedDeltaTime);
    /// accumulator에 남은 비율(0 이상 1 미만)로 두 상태를 보간해 그린다.
    virtual void render(double interpolationAlpha);
    virtual void onStop();

private:
    ApplicationConfig config_;
    bool running_ = false;
};

} // namespace meno
