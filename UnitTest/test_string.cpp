#include <gtest/gtest.h>
#include <hyue/StringUtils.h>

using namespace hyue;

TEST(StringUtils, lower) {
    EXPECT_EQ(StringUtils::lower(""), "");
    EXPECT_EQ(StringUtils::lower("a"), "a");
    EXPECT_EQ(StringUtils::lower("A"), "a");
    EXPECT_EQ(StringUtils::lower("Hello world!"), "hello world!");
    EXPECT_EQ(StringUtils::lower("你好, Big Apple"), "你好, big apple");
}

TEST(StringUtils, upper) {
    EXPECT_EQ(StringUtils::upper(""), "");
    EXPECT_EQ(StringUtils::upper("A"), "A");
    EXPECT_EQ(StringUtils::upper("a"), "A");
    EXPECT_EQ(StringUtils::upper("Hello world!"), "HELLO WORLD!");
    EXPECT_EQ(StringUtils::upper("你好, Big Apple"), "你好, BIG APPLE");
}

TEST(StringUtils, trim) {
    EXPECT_EQ(StringUtils::trim(""), "");
    EXPECT_EQ(StringUtils::trim(" A "), "A");
    EXPECT_EQ(StringUtils::trim("\t 你好\t ", true, false), "你好\t ");
    EXPECT_EQ(StringUtils::trim("\t 你好\t ", false, true), "\t 你好");
    EXPECT_EQ(StringUtils::trim("\t 你好\t "), "你好");
}

TEST(StringUtils, starts_with) {
    EXPECT_TRUE(StringUtils::starts_with("A", "A"));
    EXPECT_TRUE(StringUtils::starts_with("AB", "A"));
    EXPECT_TRUE(StringUtils::starts_with("你好", "你"));
    EXPECT_TRUE(StringUtils::starts_with("a", "A", true));
    EXPECT_TRUE(StringUtils::starts_with("A", "a", true));

    EXPECT_FALSE(StringUtils::starts_with("a", "A"));
    EXPECT_FALSE(StringUtils::starts_with("A", ""));
    EXPECT_FALSE(StringUtils::starts_with("A", "B"));
    EXPECT_FALSE(StringUtils::starts_with("AB", "B"));
    EXPECT_FALSE(StringUtils::starts_with("你好", "好"));
}

TEST(StringUtils, ends_with) {
    EXPECT_TRUE(StringUtils::ends_with("A", "A"));
    EXPECT_TRUE(StringUtils::ends_with("AB", "B"));
    EXPECT_TRUE(StringUtils::ends_with("你好", "好"));

    EXPECT_FALSE(StringUtils::ends_with("A", "B"));
    EXPECT_FALSE(StringUtils::ends_with("A", ""));
    EXPECT_FALSE(StringUtils::ends_with("你好", "你"));
    EXPECT_FALSE(StringUtils::ends_with("AB", "A"));
}

TEST(StringUtils, split) {
    EXPECT_EQ(StringUtils::split("A", ","), (StringVector{"A"}));
    EXPECT_EQ(StringUtils::split("A,B", ","), (StringVector{"A", "B"}));
    EXPECT_EQ(StringUtils::split("AA,,BB", ","), (StringVector{"AA", "", "BB"}));
    EXPECT_EQ(StringUtils::split("AA,,BB", ",", true), (StringVector{"AA","BB"}));
}

TEST(StringUtils, normalize_file_path) {
    EXPECT_EQ(StringUtils::normalize_file_path(""), "");
    EXPECT_EQ(StringUtils::normalize_file_path("A"), "A");
    EXPECT_EQ(StringUtils::normalize_file_path("A/B"), "A/B");
    EXPECT_EQ(StringUtils::normalize_file_path("A/./B"), "A/B");
    EXPECT_EQ(StringUtils::normalize_file_path("A/../B"), "B");
    EXPECT_EQ(StringUtils::normalize_file_path("./A/../B"), "B");
    EXPECT_EQ(StringUtils::normalize_file_path("./B"), "B");
    EXPECT_EQ(StringUtils::normalize_file_path("/B"), "/B");
    EXPECT_EQ(StringUtils::normalize_file_path("/B/"), "/B/");
    EXPECT_EQ(StringUtils::normalize_file_path("/B\\A"), "/B/A");
}

TEST(StringUtils, format) {
    EXPECT_EQ(StringUtils::format("%d", 1), "1");
    EXPECT_EQ(StringUtils::format("%c", 65), "A");
    EXPECT_EQ(StringUtils::format("%s!", "hello"), "hello!");
}