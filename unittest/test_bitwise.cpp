#include <gtest/gtest.h>
#include <hyue/Bitwise.h>

using namespace hyue;

TEST(Bitwise, bswap16) { EXPECT_EQ(Bitwise::bswap16(0x1122), 0x2211); }

TEST(Bitwise, get_most_significant_bit_set)
{
    EXPECT_EQ(Bitwise::get_most_significant_bit_set(0), -1);
    EXPECT_EQ(Bitwise::get_most_significant_bit_set(1), 0);
    EXPECT_EQ(Bitwise::get_most_significant_bit_set(0x80000000), 31);
    EXPECT_EQ(Bitwise::get_most_significant_bit_set(0x800F0000), 31);
}

TEST(Bitwise, get_first_pow2_from)
{
    EXPECT_EQ(Bitwise::get_first_pow2_from(0), 0);
    EXPECT_EQ(Bitwise::get_first_pow2_from(1), 1);
    EXPECT_EQ(Bitwise::get_first_pow2_from(3), 4);
    EXPECT_EQ(Bitwise::get_first_pow2_from(15), 16);
}

TEST(Bitwise, get_bit_shift)
{
    EXPECT_EQ(Bitwise::get_bit_shift(0), 0);
    EXPECT_EQ(Bitwise::get_bit_shift(1), 0);
    EXPECT_EQ(Bitwise::get_bit_shift(3), 0);
    EXPECT_EQ(Bitwise::get_bit_shift(16), 4);
    EXPECT_EQ(Bitwise::get_bit_shift(0xF4000000), 26);
}