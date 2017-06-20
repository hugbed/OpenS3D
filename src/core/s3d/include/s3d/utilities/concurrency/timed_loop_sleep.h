#ifndef S3D_UTILITIES_CONCURRENCY_TIMED_LOOP_SLEEP_H
#define S3D_UTILITIES_CONCURRENCY_TIMED_LOOP_SLEEP_H

#include "timed_loop.h"

#include <atomic>
#include <thread>

class TimedLoopSleep : public TimedLoop {
 public:
  using time_point = std::chrono::time_point<std::chrono::high_resolution_clock>;

  ~TimedLoopSleep() override = default;

  gsl::owner<TimedLoop*> clone() const override { return new TimedLoopSleep; }

  void start(Client* client, std::chrono::microseconds loopDuration) override {
    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::microseconds;
    using namespace std::chrono_literals;

    auto nextFrameTime = high_resolution_clock::now() + loopDuration;
    while (!stopLoopFlag_) {
      // sleep until time has come (thread sleep then a bit of CPU burning)
      auto sleepDuration = nextFrameTime - high_resolution_clock::now();

      // this_thread::sleep only if necessary
      if (sleepDuration >= 1ms) {
        std::this_thread::sleep_for(sleepDuration * 0.9);
      }

      // a bit of busy waiting for the remaining time
      while ((high_resolution_clock::now() - nextFrameTime) < 0us) {
      }

//      outputPerformanceMetrics(std::cout, loopDuration);
      client->callback();

      // update next frame time
      nextFrameTime += loopDuration;
    }
    // reset flag
    stopLoopFlag_ = false;
  }

  void outputPerformanceMetrics(std::ostream& outStream, std::chrono::microseconds baselineTime) {
    using std::chrono::microseconds;
    using std::chrono::duration_cast;

    auto tmpMesure = lastTimeMesure;

    auto now = std::chrono::high_resolution_clock::now();
    auto dt = now - lastTimeMesure;
    lastTimeMesure = now;

    if (tmpMesure != time_point::max()) {
      long dtl = duration_cast<microseconds>(dt).count();
      outStream << dtl - baselineTime.count() << std::endl;
    }
  }

  // todo: unit test the value of the flag through the public methods
  void stop() override { stopLoopFlag_ = true; }

 private:
  time_point lastTimeMesure{time_point::max()};

  // todo: atomic maybe slow
  std::atomic<bool> stopLoopFlag_{false};
};

#endif  // S3D_UTILITIES_CONCURRENCY_TIMED_LOOP_SLEEP_H
