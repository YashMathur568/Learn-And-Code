#include <gtest/gtest.h>
#include "CategoryUtils.h"

// ─── categoryToString ─────────────────────────────────────────────────────────

TEST(CategoryUtils, CategoryToStringAllValues) {
    EXPECT_EQ(categoryToString(Category::FOOD),          "FOOD");
    EXPECT_EQ(categoryToString(Category::TRANSPORT),     "TRANSPORT");
    EXPECT_EQ(categoryToString(Category::UTILITIES),     "UTILITIES");
    EXPECT_EQ(categoryToString(Category::ENTERTAINMENT), "ENTERTAINMENT");
    EXPECT_EQ(categoryToString(Category::HEALTH),        "HEALTH");
    EXPECT_EQ(categoryToString(Category::EDUCATION),     "EDUCATION");
    EXPECT_EQ(categoryToString(Category::OTHER),         "OTHER");
}

// ─── stringToCategory ─────────────────────────────────────────────────────────

TEST(CategoryUtils, StringToCategoryAllValues) {
    EXPECT_EQ(stringToCategory("FOOD"),          Category::FOOD);
    EXPECT_EQ(stringToCategory("TRANSPORT"),     Category::TRANSPORT);
    EXPECT_EQ(stringToCategory("UTILITIES"),     Category::UTILITIES);
    EXPECT_EQ(stringToCategory("ENTERTAINMENT"), Category::ENTERTAINMENT);
    EXPECT_EQ(stringToCategory("HEALTH"),        Category::HEALTH);
    EXPECT_EQ(stringToCategory("EDUCATION"),     Category::EDUCATION);
    EXPECT_EQ(stringToCategory("OTHER"),         Category::OTHER);
}

TEST(CategoryUtils, StringToCategoryUnknownFallsBackToOther) {
    EXPECT_EQ(stringToCategory(""),         Category::OTHER);
    EXPECT_EQ(stringToCategory("UNKNOWN"),  Category::OTHER);
    EXPECT_EQ(stringToCategory("food"),     Category::OTHER);
}

TEST(CategoryUtils, RoundTrip) {
    std::vector<Category> all = {
        Category::FOOD, Category::TRANSPORT, Category::UTILITIES,
        Category::ENTERTAINMENT, Category::HEALTH, Category::EDUCATION, Category::OTHER
    };
    for (Category c : all)
        EXPECT_EQ(stringToCategory(categoryToString(c)), c);
}

// ─── formatAmount ─────────────────────────────────────────────────────────────

TEST(CategoryUtils, FormatAmountShowsRupeeSymbol) {
    std::string result = formatAmount(100.0);
    EXPECT_EQ(result.substr(0, 3), "₹");
}

TEST(CategoryUtils, FormatAmountTwoDecimalPlaces) {
    std::string result = formatAmount(99.5);
    EXPECT_EQ(result, "₹99.50");
}

TEST(CategoryUtils, FormatAmountZero) {
    EXPECT_EQ(formatAmount(0.0), "₹0.00");
}

TEST(CategoryUtils, FormatAmountLargeValue) {
    EXPECT_EQ(formatAmount(100000.0), "₹100000.00");
}
