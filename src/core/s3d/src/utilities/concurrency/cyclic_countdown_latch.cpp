#include "s3d/utilities/concurrency/cyclic_countdown_latch.h"

void CyclicCountDownLatch::countDown() {
  std::unique_lock<std::mutex> l(countMutex_);
  {
    --count_;
    if (count_ <= 0) {
      count_ = parties_;
      semaphore_.notify();
    }
  }
}

void CyclicCountDownLatch::await() {
  semaphore_.await();
}
