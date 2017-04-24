#include "gtest/gtest.h"

#include "s3d/geometry/size.h"

TEST(geometry_size, default_creation) {
  Size s;
  EXPECT_EQ(s.getWidth(), 0);
  EXPECT_EQ(s.getHeight(), 0);
}

TEST(geometry_size, params_default_creation) {
  Size s(0, 0);
  EXPECT_EQ(s.getWidth(), 0);
  EXPECT_EQ(s.getHeight(), 0);
}

TEST(geometry_size, params_creation) {
  Size s(1, 2);
  EXPECT_EQ(s.getWidth(), 1);
  EXPECT_EQ(s.getHeight(), 2);
}

TEST(geometry_size, area) {
  Size s(2, 2);
  EXPECT_EQ(s.getArea(), 4);
}

TEST(geometry_size, equal_compare) {
  EXPECT_TRUE(Size(2, 2) == Size(2, 2));
  EXPECT_FALSE(Size(1, 1) == Size(2, 2));
}

TEST(geometry_size, not_equal_compare) {
  EXPECT_FALSE(Size(2, 2) != Size(2, 2));
  EXPECT_TRUE(Size(1, 1) != Size(2, 2));
}

TEST(geometry_size, negative_width_exception) {
  ASSERT_THROW(Size(-1, 1), negative_size_exception);
}

TEST(geometry_size, negative_height_exception) {
  ASSERT_THROW(Size(1, -1), negative_size_exception);
}

TEST(geometry_size, negative_size_exception) {
  ASSERT_THROW(Size(-1, -1), negative_size_exception);
}
