
#ifndef S3D_UTILITIES_CONCURRENCY_TIMED_LOOP_SLEEP_H
#define S3D_UTILITIES_CONCURRENCY_TIMED_LOOP_SLEEP_H

#include "timed_loop.h"

#include <atomic>
#include <thread>

class TimedLoopSleep : public TimedLoop {
 public:
  ~TimedLoopSleep() override = default;

  gsl::owner<TimedLoop*> clone() const override { return new TimedLoopSleep; }

  void start(Client* client, std::chrono::milliseconds loopDuration) override {
    while (!stopLoopFlag_) {
      using std::chrono::high_resolution_clock;
      auto t1 = high_resolution_clock::now();
      client->callback();
      auto dt = high_resolution_clock::now() - t1;
      std::this_thread::sleep_for(loopDuration - dt);
    }
    // reset flag
    stopLoopFlag_ = false;
  }

  // todo: unit test the value of the flag through the public methods
  void stop() override { stopLoopFlag_ = true; }

 private:
  // todo: atomic maybe slow
  std::atomic<bool> stopLoopFlag_{false};
};

#endif  // S3D_UTILITIES_CONCURRENCY_TIMED_LOOP_SLEEP_H
