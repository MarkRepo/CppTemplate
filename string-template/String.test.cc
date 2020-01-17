//
// Created by markfqwu on 2020-01-17.
//
#include "gtest/gtest.h"
#include "string-template/String.h"

TEST(StringTest, append){
    String<char> str;
    char ch = 'a';
    str += ch;
    EXPECT_EQ(str, "a");
}