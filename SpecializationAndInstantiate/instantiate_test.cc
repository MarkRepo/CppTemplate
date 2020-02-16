//
// Created by markfqwu on 2020-02-08.
//

#include "gtest/gtest.h"
#include "SpecializationAndInstantiate/instantiate.h"

using namespace tpl::instantiate;

TEST(InstantiateTest, f1){
    Pool pool;
    user(pool);
}

TEST(InstantiateTest, before_g){
    int a = 1;
    at_ff(a);
}

TEST(InstantiateTest, hhh){
    int a = 1;
    hhh(a);
}

TEST(InstantiateTest, hhh2){
    int a = 3;
    hhh2(a);
}

TEST(InstantiateTest, fff3){
    fff3();
}

TEST(InstantiateTest, bh){
    BX<BB> bxb;
    B1X<B1B> b1xb1;
    bh(bxb);
    b1h(b1xb1);
}