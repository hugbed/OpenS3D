//
// Created by jon on 22/03/17.
//

#ifndef S3D_UTILITIES_CONCURRENCY_CONSUMER_BARRIER_SYNC_H
#define S3D_UTILITIES_CONCURRENCY_CONSUMER_BARRIER_SYNC_H

#include "producer_barrier_sync.h"

#include <mutex>
#include <vector>

namespace s3d {
namespace concurrency {

template <class T>
class ConsumerBarrierSync {
 public:
  using Producers = std::vector<ProducerBarrierSync<T>*>;

  ConsumerBarrierSync(std::mutex* doneProducingMutex,
                      std::condition_variable* shouldConsumeCV,
                      Producers producers)
      : doneProducingMutex_(doneProducingMutex),
        shouldConsumeCV_(shouldConsumeCV),
        producers_(std::move(producers)) {}

  void startConsuming() {
    while (!shouldStopConsuming()) {
      notifyShouldProduce();
      waitUntilAllDoneProducing();
      consume();
    }
  }

 protected:
  const Producers& getProducers() { return producers_; }

 private:
  virtual void consume() = 0;  // use producer->getProduct()

  virtual bool shouldStopConsuming() {
    return std::any_of(
        std::begin(producers_), std::end(producers_),
        [](const auto& producer) { return producer->shouldStopProducing(); });
  }

  bool allAreDoneProducing() {
    return std::all_of(std::begin(producers_), std::end(producers_),
                       [](auto& p) { return p->isDoneProducing(); });
  }

  void waitUntilAllDoneProducing() {
    // wait until all producers are done producing
    std::unique_lock<std::mutex> lk(*doneProducingMutex_);
    while (!allAreDoneProducing()) {
      shouldConsumeCV_->wait(lk, [this] { return allAreDoneProducing(); });
    }

    // acknowledge done producing, set it back to default state
    for (auto& producer : producers_) {
      producer->acknowledgeDoneProducing();
    }
  }

  void notifyShouldProduce() {
    for (auto& producer : producers_) {
      producer->notifyDoneConsuming();
    }
    for (auto& producer : producers_) {
      producer->notifyShouldProduce();
    }
  }

  std::mutex* doneProducingMutex_;
  std::condition_variable* shouldConsumeCV_;
  Producers producers_;
};
}  // namespace concurrency
}  // namespace s3d

#endif  // S3D_UTILITIES_CONCURRENCY_CONSUMER_BARRIER_SYNC_H
