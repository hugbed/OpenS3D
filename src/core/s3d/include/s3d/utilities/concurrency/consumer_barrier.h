#ifndef S3D_UTILITIES_CONCURRENCY_CONSUMER_BARRIER_H
#define S3D_UTILITIES_CONCURRENCY_CONSUMER_BARRIER_H

#include "producer_barrier.h"

#include <mutex>
#include <vector>

namespace s3d {
namespace concurrency {

template <class T>
class ConsumerBarrier {
 public:
  using Producers = std::vector<ProducerBarrier<T>*>;

  ConsumerBarrier(ProducerConsumerMediator* mediator, Producers producers)
      : mediator_(mediator), producers_(std::move(producers)) {}

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
    return std::any_of(std::begin(producers_), std::end(producers_),
                       [](const auto& producer) { return producer->shouldStopProducing(); });
  }

  bool allAreDoneProducing() {
    return std::all_of(std::begin(producers_), std::end(producers_),
                       [](auto& p) { return p->isDoneProducing(); });
  }

  void waitUntilAllDoneProducing() {
    mediator_->waitUntilAllDoneProducing([this]() { return allAreDoneProducing(); });

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

  gsl::not_null<ProducerConsumerMediator*> mediator_;
  Producers producers_;
};
}  // namespace concurrency
}  // namespace s3d

#endif  // S3D_UTILITIES_CONCURRENCY_CONSUMER_BARRIER_H
