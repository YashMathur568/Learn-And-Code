#include <gtest/gtest.h>
#include "../inc/DivisorCounter.h"

class DivisorCounterTest : public ::testing::Test {
protected:
    DivisorCounter divisorCounter;
};

TEST_F(DivisorCounterTest, DivisorsOfOneReturnsOne) {
    EXPECT_EQ(divisorCounter.countDivisors(1), 1);
}

TEST_F(DivisorCounterTest, DivisorsOfPrimeReturnsTwo) {
    EXPECT_EQ(divisorCounter.countDivisors(2), 2);
    EXPECT_EQ(divisorCounter.countDivisors(3), 2);
    EXPECT_EQ(divisorCounter.countDivisors(7), 2);
    EXPECT_EQ(divisorCounter.countDivisors(13), 2);
}

TEST_F(DivisorCounterTest, DivisorsOfPerfectSquare) {
    EXPECT_EQ(divisorCounter.countDivisors(4), 3);
    EXPECT_EQ(divisorCounter.countDivisors(9), 3);
    EXPECT_EQ(divisorCounter.countDivisors(16), 5);
}

TEST_F(DivisorCounterTest, DivisorsOfCompositeNumber) {
    EXPECT_EQ(divisorCounter.countDivisors(6), 4);
    EXPECT_EQ(divisorCounter.countDivisors(12), 6);
    EXPECT_EQ(divisorCounter.countDivisors(14), 4);
    EXPECT_EQ(divisorCounter.countDivisors(15), 4);
}

TEST_F(DivisorCounterTest, DivisorsOfZeroReturnsZero) {
    EXPECT_EQ(divisorCounter.countDivisors(0), 0);
}

TEST_F(DivisorCounterTest, DivisorsOfNegativeNumberReturnsZero) {
    EXPECT_EQ(divisorCounter.countDivisors(-1), 0);
    EXPECT_EQ(divisorCounter.countDivisors(-10), 0);
}

TEST_F(DivisorCounterTest, DivisorsOfLargeNumber) {
    EXPECT_EQ(divisorCounter.countDivisors(100), 9);
    EXPECT_EQ(divisorCounter.countDivisors(1000), 16);
}
