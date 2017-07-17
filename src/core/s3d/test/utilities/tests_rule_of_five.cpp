#include "gtest/gtest.h"

#include "s3d/utilities/rule_of_five.h"

struct Probe {
  int counter{};
};

struct FakeInterface : rule_of_five_interface<FakeInterface> {};

struct FakeImplementation : public FakeInterface {
  explicit FakeImplementation(Probe* p) : p_{p} { p_->counter++; }
  ~FakeImplementation() override { p_->counter--; }
  gsl::owner<FakeInterface*> clone() const override { return new FakeImplementation(p_); }

  Probe* p_;
};

TEST(rule_of_five_interface, clone_implementation_possible) {
  Probe p;
  std::unique_ptr<FakeInterface> obj{new FakeImplementation{&p}};
  std::unique_ptr<FakeInterface> other{obj->clone()};
  EXPECT_EQ(p.counter, 2);
}

TEST(rule_of_five_interface, virtual_destructor_implementation_called) {
  Probe p;
  EXPECT_EQ(p.counter, 0);
  {
    std::unique_ptr<FakeInterface> obj{new FakeImplementation{&p}};
    EXPECT_EQ(p.counter, 1);
  }
  EXPECT_EQ(p.counter, 0);
}
