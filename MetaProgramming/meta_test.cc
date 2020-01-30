//
// Created by markfqwu on 2020-01-29.
//

#include "gtest/gtest.h"
#include "MetaProgramming/meta.h"

using namespace tpl::meta;

TEST(MetaTest, Obj_holder){
    tpl::meta::Obj_holder_test();
}

TEST(MetaTest, Holder){
    tpl::meta::Holder_test();
}

TEST(MetaTest, GetUnsignedIntegerT){
    tpl::meta::GetUnsignedIntegerT_test();
}