#include "s3d/concurrency/cyclic_countdown_latch.h"

namespace s3d {

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

}  // namespace s3d
