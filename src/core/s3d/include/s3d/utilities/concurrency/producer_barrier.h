#ifndef S3D_UTILITIES_CONCURRENCY_PRODUCER_BARRIER_H
#define S3D_UTILITIES_CONCURRENCY_PRODUCER_BARRIER_H

#include "producer_consumer_mediator.h"

#include "binary_semaphore.h"
#include "cyclic_countdown_latch.h"

#include <algorithm>
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <vector>

#include "gsl/gsl"

namespace s3d {
namespace concurrency {

// Separate class to handle thread locks and condition variables
// This way it could be implemented with other threading primitives
class ProducerConsumerBarrier : public ProducerConsumerMediator {
 public:
  ProducerConsumerBarrier(CyclicCountDownLatch* consumerLatch, BinarySemaphore* producerSemaphore)
      : consumerLatch_{consumerLatch}, producerSemaphore_{producerSemaphore} {}

  gsl::owner<ProducerConsumerMediator*> clone() const override {
    return new ProducerConsumerBarrier(consumerLatch_, producerSemaphore_);
  }

  // called/checked by consumer
  void notifyDoneConsuming() override { producerSemaphore_->notify(); }

  void notifyDoneProducing() override { consumerLatch_->countDown(); }

  void waitUntilShouldProduce() override { producerSemaphore_->await(); }

  void waitUntilShouldConsume() override { consumerLatch_->await(); }

 private:
  CyclicCountDownLatch* consumerLatch_;  // all should share this pointer
  BinarySemaphore* producerSemaphore_;
};

template <class T>
class ProducerBarrier {
 public:
  explicit ProducerBarrier(gsl::not_null<ProducerConsumerMediator*> mediator)
      : mediator_(mediator) {}
  virtual const T& getProduct() = 0;

  void startProducing() {
    // reset stop flag
    // todo: may be problem with trying to stop right after starting
    shouldStop_ = false;
    onStartProducing();
    while (!shouldStopProducing()) {
      produce();
      mediator_->notifyDoneProducing();
      mediator_->waitUntilShouldProduce();
    }
  }

  void stop() {
    shouldStop_ = true;
    mediator_->notifyDoneProducing();  // todo: not sure
  }

  virtual void produce() = 0;  // where the work happens : set product

  // called/checked by consumer
  virtual bool shouldStopProducing() { return shouldStop_; }

 private:
  virtual void onStartProducing() {}

  std::atomic<bool> shouldStop_{false};
  ProducerConsumerMediator* mediator_;
};
}  // namespace concurrency
}  // namespace s3d

#endif  // S3D_UTILITIES_CONCURRENCY_PRODUCER_BARRIER_H
