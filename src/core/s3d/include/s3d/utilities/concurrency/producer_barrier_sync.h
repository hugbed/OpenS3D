//
// Created by jon on 22/03/17.
//

#ifndef S3D_UTILITIES_CONCURRENCY_PRODUCER_BARRIER_SYNC_H
#define S3D_UTILITIES_CONCURRENCY_PRODUCER_BARRIER_SYNC_H

#include <algorithm>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

namespace s3d {
namespace concurrency {

template <class T>
class ProducerBarrierSync {
 public:
  ProducerBarrierSync() = delete;
  ProducerBarrierSync(std::mutex* doneProducingMutex,
                      std::condition_variable* shouldConsumeCV)
      : doneProducingCondition_{false},
        doneConsumingCondition_{false},
        shouldConsumeCV_{shouldConsumeCV},
        doneProducingMutex_{doneProducingMutex} {}

  virtual const T& getProduct() = 0;

  void startProducing() {
    onStartProducing();
    while (!shouldStopProducing()) {
      waitUntilProductIsConsumed();
      produce();
      notifyDoneProducing();
    }
  }

  void acknowledgeDoneProducing() { doneProducingCondition_ = false; }

  bool isDoneProducing() { return doneProducingCondition_; }

  void notifyDoneConsuming() {
    std::unique_lock<std::mutex> lk(doneConsumingMutex_);
    doneConsumingCondition_ = true;
  }

  std::mutex& getDoneConsumingMutex() { return doneConsumingMutex_; }

  void notifyShouldProduce() { shouldProduceCV_.notify_one(); }

  virtual bool shouldStopProducing() { return false; }

 private:
  virtual void produce() = 0;  // where the work happens : set product
  virtual void onStartProducing() {}

  void notifyDoneProducing() {
    {
      std::unique_lock<std::mutex> lk(*doneProducingMutex_);
      doneProducingCondition_ = true;
    }
    shouldConsumeCV_->notify_one();
  }

  void waitUntilProductIsConsumed() {
    // wait until doneConsuming
    std::unique_lock<std::mutex> lk(doneConsumingMutex_);
    while (!(doneConsumingCondition_)) {
      shouldProduceCV_.wait(lk, [this] { return doneConsumingCondition_; });
    }
    // we acknowledged it, set it back to default state
    doneConsumingCondition_ = false;
  }

  bool doneProducingCondition_;
  bool doneConsumingCondition_;
  std::condition_variable shouldProduceCV_;
  std::condition_variable* shouldConsumeCV_;
  std::mutex* doneProducingMutex_;
  std::mutex doneConsumingMutex_;
};
}  // namespace concurrency
}  // namespace s3d

#endif  // S3D_UTILITIES_CONCURRENCY_PRODUCER_BARRIER_SYNC_H
