//
// Created by markfqwu on 2020-02-02.
//

#include "gtest/gtest.h"
#include "MetaProgramming/enable_if.h"
#include <string>
#include <vector>

using std::string;
using tpl::common::DeMangle;

template< std::size_t Len, class... Types >
using Aligned_union = typename std::aligned_union<Len,Types...>::type;

TEST(MetaTest, enable_if_test){
    Aligned_union<0,int,std::string> u;

    construct(reinterpret_cast<int*>(&u));
    destroy(reinterpret_cast<int*>(&u));
    cout << DeMangle(typeid(destroy<decltype(reinterpret_cast<int*>(&u))>).name()) << endl;

    construct(reinterpret_cast<std::string*>(&u),"Hello");
    destroy(reinterpret_cast<std::string*>(&u));
    auto& func2 = destroy<decltype(reinterpret_cast<std::string*>(&u))>;
    cout << DeMangle(typeid(func2).name()) << endl; //得到的类型不是真正调用的类型，即auto的推导出错了

    A<int> a1; // OK ，匹配初等模板
    A<double> a2; // OK ，匹配部分特化
    A<double, double> a3;
    A<double, int> a4;

    cout << "A<int> type is " << DeMangle(typeid(a1).name()) << endl;
    cout << "A<double> type is " << DeMangle(typeid(a2).name()) << endl;
    cout << "A<double,double> type is " << DeMangle(typeid(a3).name()) << endl;
    cout << "A<double,int> type is " << DeMangle(typeid(a4).name()) << endl;

    int i = 0;
    GetPtr(&i);
    string s{};
    GetPtr(&s);

    //destroy_test<string, 1>(&s);
    destroy_test<string, 1>(&s);
    destroy_test<string, 2>(&s);

    //destroy_test<int, 0>(&i);

    des_test<string>(&s);
    des_test<string, int>(&s);
    std::vector<int> v;
    des_test(&v);
    des_test<std::vector<int>, int>(&v);
}