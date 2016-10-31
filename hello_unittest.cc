#include "hello.h"

#include <gtest/gtest.h>

TEST(HelloTest, GetHello) {
  EXPECT_EQ("hello world", GetHello());
}
