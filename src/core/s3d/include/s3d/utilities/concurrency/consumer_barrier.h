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
  using Mediators = std::vector<ProducerConsumerMediator*>;

  ConsumerBarrier(Mediators mediators, Producers producers)
      : mediators_(std::move(mediators)), producers_(std::move(producers)) {}

  void startConsuming() {
    while (!shouldStopConsuming()) {
      mediators_[0]->waitUntilShouldConsume();
      consume();
      notifyDoneConsuming();
    }
  }

  // stop is called on each producers
  // then it wake them up so they can stop gracefully
  void stop() {
    for (auto* producer : producers_) {
      producer->stop();
    }
    notifyDoneConsuming();
  }

  virtual void consume() = 0;  // use producer->getProduct()

 protected:
  const Producers& getProducers() { return producers_; }

  virtual bool shouldStopConsuming() {
    return std::any_of(std::begin(producers_), std::end(producers_),
                       [](auto* producer) { return producer->shouldStopProducing(); });
  }

 private:
  void notifyDoneConsuming() {
    for (auto* mediator : mediators_) {
      mediator->notifyDoneConsuming();
    }
  }

  Mediators mediators_;
  Producers producers_;
};
}  // namespace concurrency
}  // namespace s3d

#endif  // S3D_UTILITIES_CONCURRENCY_CONSUMER_BARRIER_H
