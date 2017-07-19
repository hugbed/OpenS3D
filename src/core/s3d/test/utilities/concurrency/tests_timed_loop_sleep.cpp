#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "s3d/utilities/concurrency/timed_loop_sleep.h"

#include <chrono>

class FakeTimedLoop : public TimedLoop {
  gsl::owner<FakeTimedLoop*> clone() const { return new FakeTimedLoop; }
  void start(TimedLoop::Client* /*client*/, std::chrono::microseconds /*loopDuration*/) override {}
  void stop() override {}
};

TEST(timed_loop, maybe_pause_does_nothing) {
  FakeTimedLoop loop;
  loop.maybePause();
}

TEST(timed_loop, resume_does_nothing) {
  FakeTimedLoop loop;
  loop.resume();
}

struct ClientStop : public TimedLoop::Client {
  gsl::owner<ClientStop*> clone() const { return new ClientStop; }

  TimedLoopSleep* loop{};

  void callback() override {
    if (loop != nullptr) {
      loop->stop();
      nbCalls++;
    }
  }
  int nbCalls{0};
};

struct ClientPause : public TimedLoop::Client {
  gsl::owner<ClientPause*> clone() const { return new ClientPause; }

  TimedLoopSleep* loop{};

  void callback() override {
    if (loop != nullptr) {
      loop->maybePause();
      paused = true;
      nbCalls++;
    }
  }

  std::atomic<bool> paused{false};
  int nbCalls{0};
};

TEST(timed_loop_sleep, stop_stops) {
  ClientStop c;
  TimedLoopSleep t;
  c.loop = &t;
  t.start(&c, std::chrono::microseconds(0));
  EXPECT_EQ(c.nbCalls, 1);
}

TEST(timed_loop_sleep, pause_resume) {
  ClientPause client;
  TimedLoopSleep loop;
  client.loop = &loop;

  auto t = std::thread([&loop, &client] { loop.start(&client, std::chrono::microseconds(0)); });
  while (not client.paused) {
  }
  loop.stop();
  loop.resume();

  t.join();

  EXPECT_EQ(client.nbCalls, 1);
}
