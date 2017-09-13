#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "s3d/concurrency/consumer_barrier.h"
#include "s3d/concurrency/producer_barrier.h"

using s3d::ProducerConsumerMediator;
using s3d::ProducerConsumerBarrier;
using s3d::ProducerBarrier;
using s3d::BinarySemaphore;
using s3d::CyclicCountDownLatch;

class MockProducerConsumerMediator : public s3d::ProducerConsumerMediator {
 public:
  gsl::owner<ProducerConsumerMediator*> clone() const override {
    return new MockProducerConsumerMediator;
  }

  MOCK_METHOD0(notifyShouldProduce, void());
  MOCK_METHOD0(notifyDoneConsuming, void());
  MOCK_METHOD0(notifyDoneProducing, void());
  MOCK_METHOD0(waitUntilShouldProduce, void());
  MOCK_METHOD0(waitUntilShouldConsume, void());
};

class MockCyclicCountDownLatch : public CyclicCountDownLatch {
 public:
  MockCyclicCountDownLatch(int64_t parties) : CyclicCountDownLatch(parties) {}
  MOCK_METHOD0(countDown, void());
  MOCK_METHOD0(await, void());
};

class MockBinarySemaphore : public BinarySemaphore {
 public:
  MOCK_METHOD0(await, void());
  MOCK_METHOD0(notify, void());
};

class FakeProducerInt : public s3d::ProducerBarrier<int> {
 public:
  using Base = s3d::ProducerBarrier<int>;

  explicit FakeProducerInt(s3d::ProducerConsumerMediator* mediator) : ProducerBarrier{mediator} {}
  const int& getProduct() override { return product_; }
  void produce() override { product_ = 1; }
  bool shouldStopProducing() override { return Base::shouldStopProducing() || product_ == 1; }
  int product_{0};
};

TEST(producer_barrier, wait_produce_notify) {
  MockProducerConsumerMediator mediator;
  FakeProducerInt producer(&mediator);
  EXPECT_EQ(producer.getProduct(), 0);
  EXPECT_CALL(mediator, waitUntilShouldProduce());
  EXPECT_CALL(mediator, notifyDoneProducing());
  producer.startProducing();
  EXPECT_EQ(producer.getProduct(), 1);
}

TEST(producer_consumer_barrier, done_consuming_notifies) {
  MockCyclicCountDownLatch latch(1);
  MockBinarySemaphore semaphore;
  ProducerConsumerBarrier b(&latch, &semaphore);
  EXPECT_CALL(semaphore, notify());
  b.notifyDoneConsuming();
}

TEST(producer_consumer_barrier, notify_gets_out_of_wait_until_should_produce) {
  CyclicCountDownLatch latch(1);
  BinarySemaphore semaphore;
  ProducerConsumerBarrier b(&latch, &semaphore);

  auto t = std::thread([&] { b.waitUntilShouldProduce(); });
  b.notifyDoneConsuming();
  t.join();
}

TEST(producer_consumer_barrier, produce_then_consume_order_correct_integration_test) {
  CyclicCountDownLatch latch(1);
  BinarySemaphore semaphore;
  ProducerConsumerBarrier b(&latch, &semaphore);

  int product = 0;
  auto t = std::thread([&] {
    // wait, then produce 1
    product = 1;
    b.notifyDoneProducing();
    b.waitUntilShouldProduce();

    // wait, then produce 0
    product = 0;
    b.notifyDoneProducing();
    b.waitUntilShouldProduce();
  });

  // notify to produce 1, then wait
  b.waitUntilShouldConsume();

  // consume product 1
  EXPECT_EQ(product, 1);

  // notify to produce 0, then wait
  b.notifyDoneConsuming();
  b.waitUntilShouldConsume();

  // consume product 0
  EXPECT_EQ(product, 0);
  b.notifyDoneConsuming();

  t.join();
}

class FakeConsumerInt : public s3d::ConsumerBarrier<int> {
 public:
  using Base = s3d::ConsumerBarrier<int>;

  FakeConsumerInt(Base::Mediators mediators, Producers producers)
      : ConsumerBarrier{mediators, producers} {}
  void consume() override { stopConsuming = true; }
  bool shouldStopConsuming() override { return Base::shouldStopConsuming() || stopConsuming; }

  bool stopConsuming{false};
};

TEST(consumer_barrier, consumer_loop_notify_wait_acknowledge) {
  MockProducerConsumerMediator mediator;
  FakeProducerInt producer(&mediator);
  FakeConsumerInt consumer({&mediator}, {&producer});

  EXPECT_CALL(mediator, notifyDoneConsuming());
  EXPECT_CALL(mediator, waitUntilShouldConsume());
  consumer.startConsuming();
}

TEST(consumer_barrier, producer_loop_notify_wait_acknowledge) {
  MockProducerConsumerMediator mediator;
  FakeProducerInt producer(&mediator);
  FakeConsumerInt consumer({&mediator}, {&producer});

  EXPECT_CALL(mediator, notifyDoneProducing());
  EXPECT_CALL(mediator, waitUntilShouldProduce());
  producer.startProducing();
}

TEST(consumer_barrier, should_stop_consuming_if_one_producer_said_so) {
  MockProducerConsumerMediator mediator;
  FakeProducerInt producer1(&mediator);
  FakeProducerInt producer2(&mediator);
  FakeConsumerInt consumer({&mediator}, {&producer1, &producer2});

  EXPECT_CALL(mediator, notifyDoneProducing());
  producer1.stop();
  EXPECT_TRUE(consumer.shouldStopConsuming());
}
