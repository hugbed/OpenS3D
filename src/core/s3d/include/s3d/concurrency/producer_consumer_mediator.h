#ifndef S3D_UTILITIES_CONCURRENCY_PRODUCER_CONSUMER_MEDIATOR_H
#define S3D_UTILITIES_CONCURRENCY_PRODUCER_CONSUMER_MEDIATOR_H

#include "s3d/utilities/rule_of_five.h"

namespace s3d {

class ProducerConsumerMediator : rule_of_five_interface<ProducerConsumerMediator> {
 public:
  virtual void notifyDoneConsuming() = 0;
  virtual void notifyDoneProducing() = 0;
  virtual void waitUntilShouldProduce() = 0;
  virtual void waitUntilShouldConsume() = 0;
};

}  // namespace s3d

#endif  // S3D_UTILITIES_CONCURRENCY_PRODUCER_CONSUMER_MEDIATOR_H
