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

TEST(SFINAETest, mean){
    std::vector<int> v{1,2,3,4};
    int v2[4] = {1,2,3,4};
    int* p = v2;
    mean_test(v, p, 4);
    mean2_test(v, p, 4);
}

TEST(tpl_gTest, tpl_g){
    B<int> b;
    D<int> d;
    tpl_g(&b, &d);
}

TEST(tpl_f1Test, tpl_f1){
    vector<int> v{1,2,3,4};
    short s = 3;
    Index i(1);
    tpl_f1(v, s, i);
}

TEST(IntExactTest, int_exact){
    int_exact<8> a = 7;
    int_exact<16> b = 1;
    int_exact<128> c = 11111;
    cout << "8 a type " << DeMangle(typeid(a).name()) << endl;
    cout << "16 b type " << DeMangle(typeid(b).name()) << endl;
    cout << "128 c type " << DeMangle(typeid(c).name()) << endl;
}