#ifndef S3D_UTILITIES_CONCURRENCY_CYCLIC_COUNTDOWN_LATCH_H
#define S3D_UTILITIES_CONCURRENCY_CYCLIC_COUNTDOWN_LATCH_H

#include "binary_semaphore.h"

#include <mutex>

namespace s3d {

class CyclicCountDownLatch {
 public:
  CyclicCountDownLatch(int64_t parties) : count_{parties}, parties_{parties} {}

  // notifies the semaphore only when count == 0
  virtual void countDown();
  virtual void await();

 private:
  BinarySemaphore semaphore_;
  std::mutex countMutex_;
  int64_t count_{1};
  int64_t parties_{1};
};

}  // namespace s3d

#endif  // S3D_UTILITIES_CONCURRENCY_CYCLIC_COUNTDOWN_LATCH_H
