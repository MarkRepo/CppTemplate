//
// Created by markfqwu on 2020-01-19.
//

#include "gtest/gtest.h"
#include "member.h"

using namespace tpl::member;

class Quad{};

TEST(memberTest, complex){
    complex<float> cf;
    complex<double> cd{cf};
    //complex<float> cf2{cd}; // error 没有double到float的隐式转换

    //complex<float> cf3{2.0, 3.0}; // error 没有double到float的隐式转换
    complex<double> cd2{2.0F, 3.0F};
    complex<Quad> cq;
    //complex<int> ci{cq}; // error 没有Quad到int的转化
}

TEST(BadList2Test, user){
    BadList2<int, MyAllocator> lm;
    BadList2<int, YouAllocator> ly;
    user(lm, ly);
}

TEST(List2Test, user2){
    List2<int, MyAllocator> lm;
    List2<int, YouAllocator> ly;
    user2(lm, ly);
}

TEST(FriendTest, MatrixVector){
    std::initializer_list<int> l[4] = {{1,1,1,1}, {2,2,2,2}, {3,3,3,3}, {4,4,4,4}};
    Matrix<int>  m{l};
    Vector<int>  v{1,1,1,1};
    m*v;
}