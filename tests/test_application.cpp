#include <meno/core/Application.hpp>
#include <meno/core/Clock.hpp>
#include <meno/core/Time.hpp>

#include <cassert>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <vector>

namespace {

bool near(double left, double right) {
    return std::abs(left - right) < 1e-9;
}

class TestApplication final : public meno::Application {
public:
    using Application::Application;

    std::vector<double> updateDeltas;
    std::vector<double> observedDeltas;
    std::vector<double> observedElapsedTimes;
    std::size_t renderCount = 0;
    std::vector<double> interpolationAlphas;

private:
    void update(double deltaTime) override {
        updateDeltas.push_back(deltaTime);
        observedDeltas.push_back(meno::Time::deltaTime());
        observedElapsedTimes.push_back(meno::Time::elapsedTime());
    }

    void render(double interpolationAlpha) override {
        ++renderCount;
        interpolationAlphas.push_back(interpolationAlpha);
        if (renderCount == 3) {
            stop();
        }
    }
};

} // namespace

int main() {
    using namespace std::chrono_literals;
    using TimePoint = meno::Clock::TimePoint;

    // 생성 시 0초, 이후 프레임은 5ms, 12ms, 100ms가 흐른다.
    const std::vector<TimePoint> times{TimePoint{}, TimePoint{} + 5ms,
                                       TimePoint{} + 17ms, TimePoint{} + 117ms};
    std::size_t nextTime = 0;
    meno::Clock clock([&] { return times.at(nextTime++); });

    TestApplication app({.fixedTimeStep = 0.01,
                         .maxFrameTime = 0.05,
                         .maxUpdatesPerFrame = 3});
    app.run(clock);

    assert(app.renderCount == 3);
    assert(app.interpolationAlphas.size() == 3);
    assert(near(app.interpolationAlphas[0], 0.5));
    assert(near(app.interpolationAlphas[1], 0.7));
    assert(app.interpolationAlphas[2] >= 0.0 && app.interpolationAlphas[2] < 1.0);
    // 첫 두 프레임에서 1회, 밀린 세 번째 프레임에서 상한인 3회.
    assert(app.updateDeltas.size() == 4);
    for (std::size_t index = 0; index < app.updateDeltas.size(); ++index) {
        assert(near(app.updateDeltas[index], 0.01));
        assert(near(app.observedDeltas[index], 0.01));
        assert(near(app.observedElapsedTimes[index],
                    0.01 * static_cast<double>(index + 1)));
    }
    assert(near(meno::Time::deltaTime(), 0.01));
    assert(near(meno::Time::fixedDeltaTime(), 0.01));
    assert(near(meno::Time::elapsedTime(), 0.04));
    assert(near(meno::Time::frameDeltaTime(), 0.05));
    assert(near(meno::Time::realElapsedTime(), 0.117));
    assert(!app.isRunning());
}
