#include "s3d/concurrency/binary_semaphore.h"

namespace s3d {

void BinarySemaphore::await() {
  std::unique_lock<std::mutex> lk(mutex_);
  while (!(condition_)) {
    cv_.wait(lk, [this] { return condition_; });
  }
  // we acknowledged it, set it back to default state
  condition_ = false;
}

void BinarySemaphore::notify() {
  {
    std::unique_lock<std::mutex> lk(mutex_);
    condition_ = true;
  }
  cv_.notify_one();
}

}  // namespace s3d
