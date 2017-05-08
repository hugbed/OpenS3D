#include "gtest/gtest.h"

#include "s3d/utilities/file_io.h"

class file_io_push_back_n_bytes : public ::testing::Test {
 protected:
  void SetUp() override { dummyStream.str("ABCDEFGH"); }

  void TearDown() override {
    dummyStream.clear();
    buf.clear();
  }

  std::istringstream dummyStream;
  std::vector<uint8_t> buf;
};

TEST_F(file_io_push_back_n_bytes, real_size_in_two_passes) {
  s3d::file_io::push_back_n_bytes(dummyStream, 4LL, buf);
  EXPECT_EQ(buf.size(), 4);
  EXPECT_EQ(buf[0], 'A');
  EXPECT_EQ(buf[1], 'B');
  EXPECT_EQ(buf[2], 'C');
  EXPECT_EQ(buf[3], 'D');

  buf.clear();
  s3d::file_io::push_back_n_bytes(dummyStream, 4LL, buf);
  EXPECT_EQ(buf.size(), 4);
  EXPECT_EQ(buf[0], 'E');
  EXPECT_EQ(buf[1], 'F');
  EXPECT_EQ(buf[2], 'G');
  EXPECT_EQ(buf[3], 'H');
}

TEST_F(file_io_push_back_n_bytes, one_pass_too_many_not_more_data) {
  for (auto i = 0; i < 3; i++) {
    s3d::file_io::push_back_n_bytes(dummyStream, 4LL, buf);
    buf.clear();
  }
  EXPECT_EQ(buf.size(), 0);
}

TEST_F(file_io_push_back_n_bytes, two_pass_too_many_not_more_data) {
  for (auto i = 0; i < 4; i++) {
    s3d::file_io::push_back_n_bytes(dummyStream, 4LL, buf);
    buf.clear();
  }
  EXPECT_EQ(buf.size(), 0);
}

class file_io_read_n_bytes : public ::testing::Test {
 protected:
  void SetUp() override {
    dummyStream.str("ABCDEFGH");
    buf.resize(4);
  }

  void TearDown() override {
    dummyStream.clear();
    buf.clear();
  }

  std::istringstream dummyStream;
  std::vector<uint8_t> buf;
};

TEST_F(file_io_read_n_bytes, real_size_in_two_passes) {
  s3d::file_io::read_n_bytes(dummyStream, 4LL, std::begin(buf));
  EXPECT_EQ(buf.size(), 4);
  EXPECT_EQ(buf[0], 'A');
  EXPECT_EQ(buf[1], 'B');
  EXPECT_EQ(buf[2], 'C');
  EXPECT_EQ(buf[3], 'D');

  s3d::file_io::read_n_bytes(dummyStream, 4LL, std::begin(buf));
  EXPECT_EQ(buf.size(), 4);
  EXPECT_EQ(buf[0], 'E');
  EXPECT_EQ(buf[1], 'F');
  EXPECT_EQ(buf[2], 'G');
  EXPECT_EQ(buf[3], 'H');
}

TEST_F(file_io_read_n_bytes, one_pass_too_many_not_more_data) {
  for (auto i = 0; i < 3; i++) {
    s3d::file_io::read_n_bytes(dummyStream, 4LL, std::begin(buf));
  }
  EXPECT_EQ(buf[3], 'H');
}

TEST_F(file_io_read_n_bytes, two_pass_too_many_not_more_data) {
  for (auto i = 0; i < 4; i++) {
    s3d::file_io::read_n_bytes(dummyStream, 4LL, std::begin(buf));
  }
  EXPECT_EQ(buf[3], 'H');
}

TEST(file_io_write_bytes, write_abcd) {
  std::ostringstream stream;
  std::vector<uint8_t> values = {'A', 'B', 'C', 'D'};
  s3d::file_io::write_bytes(stream, values);
  auto written = stream.str();
  EXPECT_EQ(written[0], 'A');
  EXPECT_EQ(written[1], 'B');
  EXPECT_EQ(written[2], 'C');
  EXPECT_EQ(written[3], 'D');
}

TEST(file_io_write_bytes, write_ab_twice) {
  std::ostringstream stream;
  std::vector<uint8_t> values = {'A', 'B'};
  s3d::file_io::write_bytes(stream, values);
  s3d::file_io::write_bytes(stream, values);
  auto written = stream.str();
  EXPECT_EQ(written[0], 'A');
  EXPECT_EQ(written[1], 'B');
  EXPECT_EQ(written[2], 'A');
  EXPECT_EQ(written[3], 'B');
}

TEST(file_io_write_bytes, write_abcd_iterators) {
  std::ostringstream stream;
  std::vector<uint8_t> values = {'A', 'B', 'C', 'D'};
  s3d::file_io::write_bytes(stream, std::begin(values), std::end(values));
  auto written = stream.str();
  EXPECT_EQ(written[0], 'A');
  EXPECT_EQ(written[1], 'B');
  EXPECT_EQ(written[2], 'C');
  EXPECT_EQ(written[3], 'D');
}

TEST(file_io_read_bytes, all_values_are_present) {
  std::istringstream dummyStream;
  dummyStream.str("ABCD");
  std::vector<uint8_t> buf;
  s3d::file_io::read_bytes(dummyStream, back_inserter(buf));
  EXPECT_EQ(buf.size(), 4);
  EXPECT_EQ(buf[0], 'A');
  EXPECT_EQ(buf[1], 'B');
  EXPECT_EQ(buf[2], 'C');
  EXPECT_EQ(buf[3], 'D');
}