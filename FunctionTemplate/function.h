//
// Created by markfqwu on 2020-01-28.
//

#ifndef CPPTEMPLATE_FUNCTION_H
#define CPPTEMPLATE_FUNCTION_H

#include <iostream>
#include <string>
#include <typeinfo>
#include <utility>

#include "CommonTools/typeinfodemangle.h"

namespace tpl{
namespace function{

using std::string;
using std::cout;
using std::endl;
using std::pair;
using tpl::common::DeMangle;
using tpl::common::DeMangleToCout;

//编译器可以从一次调用中推断类型和非类型模版实参，前提是函数实参列表唯一标识出模版实参集合，如：
template <typename T, int max>
class Buffer{
    T buf[max];
};

template <typename T, int max>
T& lookup(Buffer<T,max>& b, const char* p){
    static T t;
    cout << "in lookup, T is " << DeMangle(typeid(T).name()) << ", max is " << max << endl;
    return t;
}

void f(Buffer<string, 128>& buf, const char* p) {
    cout << "call f" << endl;
    auto r = lookup(buf, p); // T 是string， max是128
    cout << "after call f, r typeinfo is " << DeMangle(typeid(r).name()) << endl;
};

//如果需要基于推断出的类型创建一个对象，常用的方法是通过调用一个函数来进行推断：
template <typename T1, typename T2>
pair<T1, T2> my_make_pair(T1 a, T2 b){
    cout << "in my_make_pair , t1 typeid " << DeMangle(typeid(a).name()) << ", t2 typeid " << DeMangle(typeid(b).name()) << endl;
    return {a, b};
};

//如果不能从函数实参推断出模版实参，则必需显示指定
template <typename T>
T* create(){
    cout << "in create(), T typeid " << DeMangle(typeid(T).name()) << endl;
    return new T;
}



}//namespace function
}//namespace tpl



#endif //CPPTEMPLATE_FUNCTION_H
