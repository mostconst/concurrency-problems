#include "leibnitz_pi.h"
#include <gtest/gtest.h>
#include <cstdlib>

const double correct_pi = 3.14159265358979323846;
const std::vector<std::pair<int, double>> testCases = {{1000, 1E-2}, {10'000, 1E-3}, {100'000, 1E-4},
                                                       {10'000'000, 1E-6}, {1'000'000'000, 1E-8}};

TEST(TestPi, TestST)
{
  for (const auto& c: testCases)
  {
    double res = std::abs(leibnitzPiST(c.first));
    EXPECT_TRUE(std::abs(res - correct_pi) < c.second) << "calculated pi is " << res;
  }
}

TEST(TestPi, TestMTCorrect)
{
  for (const auto& c: testCases)
  {
    double res = std::abs(leibnitzPiMT(c.first, 5));
    EXPECT_TRUE(std::abs(res - correct_pi) < c.second) << "calculated pi is " << res;
  }
}

TEST(TestPi, TestMTExtremes)
{
  {
    double res = std::abs(leibnitzPiMT(1000, 1));
    EXPECT_TRUE(std::abs(res - correct_pi) < 1E-2) << "calculated pi is " << res;
  }
  {
    double res = std::abs(leibnitzPiMT(1000, 2000));
    EXPECT_TRUE(std::abs(res - correct_pi) < 1E-2) << "calculated pi is " << res;
  }
}