#ifndef S3D_UTILITIES_CONCURRENCY_PRODUCER_BARRIER_H
#define S3D_UTILITIES_CONCURRENCY_PRODUCER_BARRIER_H

#include "producer_consumer_mediator.h"

#include <algorithm>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <vector>

#include <gsl/gsl>

namespace s3d {
namespace concurrency {

// Separate class to handle thread locks and condition variables
// This way it could be implemented with other threading primitives
class ProducerConsumerBarrier : public ProducerConsumerMediator {
 public:
  ProducerConsumerBarrier(std::mutex* doneProducingMutex, std::condition_variable* shouldConsumeCV)
      : doneProducingMutex_{doneProducingMutex}, shouldConsumeCV_{shouldConsumeCV} {}

  gsl::owner<ProducerConsumerMediator*> clone() const override {
    return new ProducerConsumerBarrier(doneProducingMutex_, shouldConsumeCV_);
  }

  // called/checked by consumer
  void acknowledgeDoneProducing() override { doneProducingCondition_ = false; }
  bool isDoneProducing() override { return doneProducingCondition_; }
  bool isDoneConsuming() override { return doneConsumingCondition_; }
  void notifyShouldProduce() override { shouldProduceCV_.notify_one(); }

  void notifyDoneConsuming() override {
    std::unique_lock<std::mutex> lk(doneConsumingMutex_);
    doneConsumingCondition_ = true;
  }

  void waitUntilAllDoneProducing(std::function<bool()> allDoneProducingCheck) override {
    std::unique_lock<std::mutex> lk(*doneProducingMutex_);
    while (!allDoneProducingCheck()) {
      shouldConsumeCV_->wait(lk, [&allDoneProducingCheck] { return allDoneProducingCheck(); });
    }
  }

  // called by producer
  void notifyDoneProducing() override {
    {
      std::unique_lock<std::mutex> lk(*doneProducingMutex_);
      doneProducingCondition_ = true;
    }
    shouldConsumeCV_->notify_one();
  }

  void waitUntilShouldProduce() override {
    // wait until doneConsuming
    std::unique_lock<std::mutex> lk(doneConsumingMutex_);
    while (!(doneConsumingCondition_)) {
      shouldProduceCV_.wait(lk, [this] { return doneConsumingCondition_; });
    }
    // we acknowledged it, set it back to default state
    doneConsumingCondition_ = false;
  }

 private:
  gsl::not_null<std::mutex*> doneProducingMutex_;
  gsl::not_null<std::condition_variable*> shouldConsumeCV_;
  bool doneProducingCondition_{false};
  bool doneConsumingCondition_{false};
  std::condition_variable shouldProduceCV_;
  std::mutex doneConsumingMutex_;
};

template <class T>
class ProducerBarrier {
 public:
  ProducerBarrier() = delete;
  explicit ProducerBarrier(ProducerConsumerMediator* mediator) : mediator_(mediator) {}
  virtual const T& getProduct() = 0;

  void startProducing() {
    onStartProducing();
    while (!shouldStopProducing()) {
      mediator_->waitUntilShouldProduce();
      produce();
      mediator_->notifyDoneProducing();
    }
  }

  // called/checked by consumer
  void acknowledgeDoneProducing() { mediator_->acknowledgeDoneProducing(); }
  bool isDoneProducing() { return mediator_->isDoneProducing(); }
  void notifyShouldProduce() { mediator_->notifyShouldProduce(); }
  void notifyDoneConsuming() { mediator_->notifyDoneConsuming(); }
  virtual bool shouldStopProducing() { return false; }

 private:
  virtual void produce() = 0;  // where the work happens : set product
  virtual void onStartProducing() {}

  gsl::not_null<ProducerConsumerMediator*> mediator_;
};
}  // namespace concurrency
}  // namespace s3d

#endif  // S3D_UTILITIES_CONCURRENCY_PRODUCER_BARRIER_H
