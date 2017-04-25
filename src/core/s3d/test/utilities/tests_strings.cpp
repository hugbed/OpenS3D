#include "gtest/gtest.h"

#include "s3d/utilities/strings.h"

TEST(string_split, two_words_coma) {
  std::string valuesStr{"one,two"};
  std::vector<std::string> splitStr;
  s3d::split(valuesStr, ',', back_inserter(splitStr));
  EXPECT_EQ(splitStr.size(), 2);
  EXPECT_EQ(splitStr[0], std::string("one"));
  EXPECT_EQ(splitStr[1], std::string("two"));
}

TEST(string_split, empty_size_0) {
  std::string valuesStr{""};
  std::vector<std::string> splitStr;
  s3d::split(valuesStr, ',', back_inserter(splitStr));
  ASSERT_EQ(splitStr.size(), 0);
}

TEST(string_split, coma_only_1_empty_string) {
  std::string valuesStr{","};
  std::vector<std::string> splitStr;
  s3d::split(valuesStr, ',', back_inserter(splitStr));
  ASSERT_EQ(splitStr.size(), 1);
  EXPECT_EQ(splitStr[0], std::string(""));
}

TEST(string_split, two_comas_only_2_empty_strings) {
  std::string valuesStr{",,"};
  std::vector<std::string> splitStr;
  s3d::split(valuesStr, ',', back_inserter(splitStr));
  ASSERT_EQ(splitStr.size(), 2);
  EXPECT_EQ(splitStr[0], std::string(""));
  EXPECT_EQ(splitStr[1], std::string(""));
}

TEST(string_split, one_then_comma) {
  std::string valuesStr{"one,"};
  std::vector<std::string> splitStr;
  s3d::split(valuesStr, ',', back_inserter(splitStr));
  ASSERT_EQ(splitStr.size(), 1);
  EXPECT_EQ(splitStr[0], std::string("one"));
}

TEST(string_split, one_no_delimiter_returns_one) {
  std::string valuesStr{"one"};
  std::vector<std::string> splitStr;
  s3d::split(valuesStr, ',', back_inserter(splitStr));
  ASSERT_EQ(splitStr.size(), 1);
  EXPECT_EQ(splitStr[0], std::string("one"));
}

TEST(string_split, empty_string) {
  std::string valuesStr{""};
  std::vector<std::string> splitStr;
  s3d::split(valuesStr, ',', back_inserter(splitStr));
  ASSERT_EQ(splitStr.size(), 0);
}
