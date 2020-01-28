//
// Created by markfqwu on 2020-01-28.
//

#include "gtest/gtest.h"
#include "function.h"

using namespace tpl::function;

TEST(FunctionTest, lookup){
    Buffer<string , 128> buf;
    char c = 'a';
    const char* p = &c;
    tpl::function::f(buf, p);
}

TEST(FunctionTest, mymakepair){

    auto x = my_make_pair(1,2);
    auto y = my_make_pair(string("New York"), 7.7);
}

TEST(FunctionTest, create){
    int* p = create<int>();
    //int* q = create(); //错误，无法推断模版实参
}
