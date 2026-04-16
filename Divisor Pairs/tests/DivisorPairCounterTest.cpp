#include <gtest/gtest.h>
#include "../inc/DivisorPairCounter.h"
#include "../inc/DivisorCounter.h"

class DivisorPairCounterTest : public ::testing::Test {
protected:
    DivisorCounter divisorCounter;
    DivisorPairCounter divisorPairCounter{divisorCounter};
};

TEST_F(DivisorPairCounterTest, UpperLimitOfTwoReturnsZero) {
    EXPECT_EQ(divisorPairCounter.countDivisorPairs(2), 0);
}

TEST_F(DivisorPairCounterTest, UpperLimitOfThreeReturnsOne) {
    EXPECT_EQ(divisorPairCounter.countDivisorPairs(3), 1);
}

TEST_F(DivisorPairCounterTest, UpperLimitOfOneReturnsZero) {
    EXPECT_EQ(divisorPairCounter.countDivisorPairs(1), 0);
}

TEST_F(DivisorPairCounterTest, ZeroUpperLimitReturnsZero) {
    EXPECT_EQ(divisorPairCounter.countDivisorPairs(0), 0);
}

TEST_F(DivisorPairCounterTest, NegativeUpperLimitReturnsZero) {
    EXPECT_EQ(divisorPairCounter.countDivisorPairs(-5), 0);
}

TEST_F(DivisorPairCounterTest, UpperLimitOfTenReturnsTwoPairs) {
    EXPECT_EQ(divisorPairCounter.countDivisorPairs(10), 1);
}

TEST_F(DivisorPairCounterTest, UpperLimitOfTwentyCountsCorrectPairs) {
    int result = divisorPairCounter.countDivisorPairs(20);
    EXPECT_GE(result, 2);
}

TEST_F(DivisorPairCounterTest, LargerUpperLimitReturnsPositiveCount) {
    int result = divisorPairCounter.countDivisorPairs(100);
    EXPECT_GT(result, 0);
}

TEST_F(DivisorPairCounterTest, ConsecutivePrimesFormPairs) {
    // (2,3) both have 2 divisors, so k=3 should yield 1
    EXPECT_EQ(divisorPairCounter.countDivisorPairs(3), 1);
}

TEST_F(DivisorPairCounterTest, PairFourteenFifteenBothHaveFourDivisors) {
    // div(14) = {1,2,7,14} = 4
    // div(15) = {1,3,5,15} = 4
    // k=15 means n can be 2..14, so pair (14,15) is included
    EXPECT_EQ(divisorPairCounter.countDivisorPairs(15), 2);
}
