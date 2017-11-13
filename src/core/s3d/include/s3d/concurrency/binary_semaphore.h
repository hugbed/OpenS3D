#ifndef S3D_UTILITIES_CONCURRENCY_BINARY_SEMAPHORE_H
#define S3D_UTILITIES_CONCURRENCY_BINARY_SEMAPHORE_H

#include <condition_variable>
#include <mutex>

namespace s3d {

class BinarySemaphore {
 public:
  virtual void await();
  virtual void notify();

 private:
  std::mutex mutex_;
  std::condition_variable cv_;
  bool condition_{false};
};

}  // namespace s3d

#endif  // S3D_UTILITIES_CONCURRENCY_BINARY_SEMAPHORE_H
