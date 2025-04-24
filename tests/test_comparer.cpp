#include <gtest/gtest.h>
#include "comparer.h"

TEST(ComparerTest, CompareIdenticalFiles) {
    std::string file1 = "Hello world";
    std::string file2 = "Hello world";
    EXPECT_TRUE(compare_files(file1, file2));
}

TEST(ComparerTest, CompareDifferentFiles) {
    std::string file1 = "Hello world";
    std::string file2 = "Goodbye world";
    EXPECT_FALSE(compare_files(file1, file2));
}
