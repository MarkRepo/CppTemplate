//
// Created by markfqwu on 2020-01-28.
//

#include "gtest/gtest.h"
#include "typeinfodemangle.h"

using namespace tpl::common;

TEST(TypeInfoDemangleTest, DeMangle){
    string demangle_str = tpl::common::DeMangle("NSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEEE");
    tpl::common::DeMangleToCout("NSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEEE");
    ASSERT_EQ(demangle_str, "std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char> >");
}
