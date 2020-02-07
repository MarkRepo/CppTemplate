//
// Created by markfqwu on 2020-02-07.
//

#include "gtest/gtest.h"
#include "MetaProgramming/unit.h"

#include <iostream>

using std::cout;
using std::endl;

using namespace tpl::unit;

TEST(UnitTest, unit){
    auto distance = 10.0_m;
    auto time = 20.0_s;
    auto speed = distance/time;

    //if(speed == 20);
    //if(speed == distance);
    if(speed == 10.0_m/20.0_s){

    }

    Quantity<MpS2> acceleration = distance/square(time);// MpS2 表示m/(s*s)
    cout << "speed==" << speed << ", acceleration== "<< acceleration << endl;
}

TEST(UnitTest, b3){
    auto b3 = 201_b3;
    ASSERT_EQ(b3, 19);

    auto b0 = 222_b3;
    ASSERT_EQ(b0, 26);

    auto b1 = 1211_b3;
    ASSERT_EQ(b1, 49);
}