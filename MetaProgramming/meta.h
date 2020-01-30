//
// Created by markfqwu on 2020-01-29.
//

#ifndef CPPTEMPLATE_META_H
#define CPPTEMPLATE_META_H

#include "CommonTools/typeinfodemangle.h"

#include <vector>
#include <string>
#include <iostream>
#include <array>
#include <type_traits>

using tpl::common::DeMangle;
using tpl::common::DeMangleToCout;
using std::string;
using std::vector;
using std::cout;
using std::endl;
using std::array;

namespace tpl{
namespace meta {

//元编程技术的两个目的
//1 提高类型安全
//2 提高运行时性能

//范型程序设计本质是一种设计哲学，主要关注对实参要求的定义

//元编程就是编程，重点是计算，通常包括选择和某种形式的迭代，元编程实现复杂度的四个等级
//1 无计算（仅仅传递类型和值实参）
//2 不使用编译时检测和迭代的（类型货值上的）简单计算，如，bool类型的&&
//3 使用显示编译时检测的计算，如，编译时if
//4 使用编译时迭代的计算（以递归形式呈现）

//元编程包含模版元编程和constexpr函数
//类型函数：接受至少一个类型参数或至少生成一个类型结果的编译时函数，即参数在编译时已知，结果在编译时可用。

//考虑"在栈中分配小对象，在堆中分配大对象"的对象分配构想的实现

template <typename T>
class On_heap {
public:
    On_heap() : p(new T) {cout<<"On heap" << endl;} // 分配
    ~On_heap() { delete p; } // 释放
    T &operator*() {
        cout << "on heap dereference" << endl;
        return *p;
    }

    T *operator->() { return p; }

    On_heap(const On_heap &) = delete;  //阻止拷贝
    On_heap operator=(const On_heap &) = delete;

private:
    T *p; //指向自由存储空间中对象的指针
};

template <typename T>
class Scoped{
public:
    Scoped(){cout << "in Scoped" << endl;}
    T& operator*(){
        cout << "on Scoped dereference" << endl;
        return x_;
    }
    T* operator->(){ return &x_;}

    Scoped(const Scoped&) = delete; //阻止拷贝
    Scoped operator=(const Scoped&) = delete;
private:
    T x_;
};

constexpr int on_stack_max = sizeof(std::string);

template <typename T>
class Obj_holder{
public:
    //std::conditional, 如果第一个参数为true, 结果为第二个参数，否则为第三个参数
    using type = typename std::conditional<(sizeof(T) <= on_stack_max), Scoped<T>, On_heap<T> >::type;
};

void Obj_holder_test(){
    cout << "Obj_holder_test" << endl;
    typename Obj_holder<double>::type v1;   //栈中分配
    typename Obj_holder<std::array<double, 200>>::type v2; //自由存储中分配

    *v1 = 7.7;
    (*v2)[77] = 9.9;
}

//引入模版别名来隐藏::type的实现细节，这是一个类型函数看起来更像一个返回类型的函数
template <typename T>
using Holder = typename Obj_holder<T>::type;

void Holder_test(){
    cout << "Holder_test" << endl;
    Holder<double> v1;
    Holder<array<double, 200>> v2;
    *v1 = 7.7;
    (*v2)[77] = 9.9;
}

//上述例子很好的展示了范型编程和模版元编程是如何要求我们为一个通用构想（本例中是对象分配的构想）的不同实现设计一致的接口的。

//当仅有一种候选可能是合法类型时，不应使用别名：
class MyError{
public:
    MyError(){cout << "this is MyError type" << endl;}
};

template <typename T>
struct Error{
    using type = MyError;
};

template <typename T>
using GetUnsignedIntegerT  = typename std::conditional<std::is_integral<T>::value, std::make_unsigned<T>, Error<T> >::type;

template <typename T>
using Make_unsigned = typename std::make_unsigned<T>::type;

template <typename T>
using Make_Error = typename Error<T>::type;

//此时可以使用显示的，面向实现的::type风格，或者引入一个类型函数Delay, 来将类型函数的求值延迟到使用时：
template <template<typename...>class F, typename... Args>
using Delay = F<Args...>;

template <typename T>
using GetUnsignedIntegerT_delay = std::conditional< std::is_integral<T>::value, Delay<Make_unsigned, T>, Error<T> >;


void GetUnsignedIntegerT_test(){
    GetUnsignedIntegerT<int> a;
    GetUnsignedIntegerT_delay<int> b;

    GetUnsignedIntegerT<std::string> c{};
    //GetUnsignedIntegerT_delay<float> d;  //编译错误。。。
    cout << "a type " << DeMangle(typeid(a).name()) << endl;
    cout << "b type " << DeMangle(typeid(b).name()) << endl;
    cout << "c type " << DeMangle(typeid(c).name()) << endl; //输出的类型预期是MyError,实际是Error<string>
    //cout << "d type " << DeMangle(typeid(d).name());
}

}// namespace meta
}// namespace tpl
#endif //CPPTEMPLATE_META_H
