//
// Created by markfqwu on 2020-01-17.
//
#include "gtest/gtest.h"
#include "string-template/String.h"

using namespace wfq;

TEST(StringTest, append){
    String<char> str;
    char ch = 'a';
    str += ch;
}
