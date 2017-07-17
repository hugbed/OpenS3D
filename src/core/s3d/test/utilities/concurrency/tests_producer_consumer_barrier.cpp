#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "s3d/utilities/concurrency/producer_barrier.h"
#include "s3d/utilities/concurrency/consumer_barrier.h"

using s3d::concurrency::ProducerConsumerMediator;
using s3d::concurrency::ProducerConsumerBarrier;
using s3d::concurrency::ProducerBarrier;

class MockProducerConsumerMediator : public s3d::concurrency::ProducerConsumerMediator {
 public:
  gsl::owner<ProducerConsumerMediator*> clone() const override {
    return new MockProducerConsumerMediator;
  }

  MOCK_METHOD0(isDoneProducing, bool());
  MOCK_METHOD0(isDoneConsuming, bool());
  MOCK_METHOD0(acknowledgeDoneProducing, void());
  MOCK_METHOD0(notifyShouldProduce, void());
  MOCK_METHOD0(notifyDoneConsuming, void());
  MOCK_METHOD1(waitUntilAllDoneProducing, void(std::function<bool()> allDoneProducingCheck));
  MOCK_METHOD0(notifyDoneProducing, void());
  MOCK_METHOD0(waitUntilShouldProduce, void());
};

class FakeProducerInt : public s3d::concurrency::ProducerBarrier<int> {
 public:
  explicit FakeProducerInt(s3d::concurrency::ProducerConsumerMediator* mediator)
      : ProducerBarrier{mediator} {}
  const int& getProduct() override { return product_; }
  void produce() override { product_ = 1; }
  bool shouldStopProducing() override { return product_ == 1; }
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

TEST(producer_consumer_barrier, notifies_sets_done_consuming) {
  std::mutex m;
  std::condition_variable c;
  ProducerConsumerBarrier b(&m, &c);
  b.notifyDoneConsuming();
  b.notifyShouldProduce();
  EXPECT_EQ(b.isDoneConsuming(), true);
}

TEST(producer_consumer_barrier, notify_gets_out_of_wait_until_should_produce) {
  std::mutex m;
  std::condition_variable c;
  ProducerConsumerBarrier b(&m, &c);

  auto t = std::thread([&] { b.waitUntilShouldProduce(); });
  b.notifyDoneConsuming();
  b.notifyShouldProduce();
  t.join();
}

TEST(producer_consumer_barrier, produce_then_consume_order_correct_integration_test) {
  std::mutex m;
  std::condition_variable c;
  ProducerConsumerBarrier b(&m, &c);

  int product = 0;
  auto t = std::thread([&] {
    // wait, then produce 1
    b.waitUntilShouldProduce();
    product = 1;
    b.notifyDoneProducing();

    // wait, then produce 0
    b.waitUntilShouldProduce();
    product = 0;
    b.notifyDoneProducing();
  });

  // notify to produce 1, then wait
  b.notifyDoneConsuming();
  b.notifyShouldProduce();
  b.waitUntilAllDoneProducing([&] { return b.isDoneProducing(); });

  // consume product 1
  EXPECT_EQ(product, 1);
  b.acknowledgeDoneProducing();

  // notify to produce 0, then wait
  b.notifyDoneConsuming();
  b.notifyShouldProduce();
  b.waitUntilAllDoneProducing([&] { return b.isDoneProducing(); });

  // consume product 0
  EXPECT_EQ(product, 0);
  b.acknowledgeDoneProducing();

  t.join();
}

class FakeConsumerInt : public s3d::concurrency::ConsumerBarrier<int> {
 public:
  FakeConsumerInt(s3d::concurrency::ProducerConsumerMediator* mediator, Producers producers)
      : ConsumerBarrier{mediator, producers} {}
  void consume() override { stopConsuming = true; }
  bool shouldStopConsuming() override { return stopConsuming; }
  bool stopConsuming{false};
};

TEST(consummer_barrier, consumer_loop_notify_wait_acknowledge) {
  MockProducerConsumerMediator mediator;
  FakeProducerInt producer(&mediator);
  FakeConsumerInt consumer(&mediator, {&producer});

  using ::testing::_;
  EXPECT_CALL(mediator, notifyDoneConsuming());
  EXPECT_CALL(mediator, notifyShouldProduce());
  EXPECT_CALL(mediator, waitUntilAllDoneProducing(_));
  EXPECT_CALL(mediator, acknowledgeDoneProducing());
  consumer.startConsuming();
}
