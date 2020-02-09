//
// Created by markfqwu on 2020-02-08.
//

#include "gtest/gtest.h"
#include "SpecializationAndInstantiate/specialization.h"

using namespace tpl::specialization;

TEST(SpecializationTest, MapCmp){
    Compare_test();
}

TEST(SpecializationTest, tplarg){
    tplarg_test();
}

TEST(SpecializationTest, LessTest){
    int a = 1, b=2;
    Less(a,b);
    Less1(a,b);
    Less2(a,b);
    const char* a1 = "aaa";
    const char* b1 = "bbb";
    Less(a1, b1);
    Less1(a1, b1);
    Less2(a1, b1);

    ASSERT_EQ(max_value<int>(), 1);
    ASSERT_EQ(max_value<char>(),'a');
}