#pragma once

#include <cstddef>

namespace meno {

class Clock;

struct ApplicationConfig {
    double fixedTimeStep = 1.0 / 60.0;
    double maxFrameTime = 0.25;
    std::size_t maxUpdatesPerFrame = 8;
    /// 초당 최대 프레임 수. 0이면 제한하지 않는다.
    unsigned int framerateLimit = 0;
};

/// 고정 timestep 게임 루프의 기본 클래스.
class Application {
public:
    explicit Application(ApplicationConfig config = {});
    virtual ~Application() = default;

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    /// 콜백의 예외는 상태를 정리한 뒤 재전파한다. onStop은 정상 종료에서만 호출된다.
    /// 이미 실행 중인 Application에 다시 호출하면 std::logic_error를 던진다.
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
