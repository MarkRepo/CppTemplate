//
// Created by markfqwu on 2020-02-02.
//

#ifndef CPPTEMPLATE_ENABLE_IF_H
#define CPPTEMPLATE_ENABLE_IF_H

#include <type_traits>
#include <iostream>
#include <string>

#include "CommonTools/typeinfodemangle.h"

using std::cout;
using std::endl;

using tpl::common::DeMangle;

//std::enable_if 元函数是活用 SFINAE ，基于类型特性条件性地从重载决议移除函数，并对不同类型特性提供分离的函数重载与特化的便利方法。
//std::enable_if 可用作额外的函数参数（不可应用于运算符重载）、返回类型（不可应用于构造函数与析构函数），或类模板或函数模板形参。

//1. 函数模版不能偏特化，只能全特化
//2. 函数模版特化时可以使用默认模版实参

namespace detail {
    struct inplace_t {
    };
}

//重载new操作符必需在全局命名空间
void* operator new(std::size_t, void* p, detail::inplace_t) {
    return p;
}

namespace tpl {
namespace enable {

template<bool B, typename T = void>
using Enable_if = typename std::enable_if<B, T>::type;

template<typename T>
Enable_if<!std::is_class<T>::value, T> *
GetPtr(T *) {
    cout << "T is not class type, T type is " << DeMangle(typeid(T).name()) << endl;
}

template<typename T>
Enable_if<std::is_class<T>::value, T> *
GetPtr(T *) {
    cout << "T is class type, T type is " << DeMangle(typeid(T).name()) << endl;
}

// #1 ，通过返回类型使用
template<class T, class... Args>
typename std::enable_if<std::is_trivially_constructible<T, Args &&...>::value>::type
construct(T *t, Args &&... args) {
    std::cout << "constructing trivially constructible T\n";
}

// #2
template<class T, class... Args>
Enable_if<!std::is_trivially_constructible<T, Args &&...>::value> // 使用帮助类型
construct(T *t, Args &&... args) {
    std::cout << "constructing non-trivially constructible T\n";
    new(t, detail::inplace_t{}) T(args...);
}

// #3 ，通过形参启用， 使用一个带默认参数的形参
template<class T>
void destroy(T *t,
             typename std::enable_if<std::is_trivially_destructible<T>::value>::type * = 0) {
    std::cout << "destroying trivially destructible T\n";
}

// #4 ，通过模板形参启用
template<class T,
        typename std::enable_if<
                !std::is_trivially_destructible<T>{} &&
                (std::is_class<T>{} || std::is_union<T>{}),
                int>::type = 0>
void destroy(T *t) {
    std::cout << "destroying non-trivially destructible T\n";
    t->~T();
}

template<typename T, int N = 2>
void destroy_test(T *t) {
    cout << "destroy_test general type, N is " << N << endl;
}

template<typename T, Enable_if<!std::is_class<T>::value, int> = 0>
//template <typename T, int = 0>
void destroy_test(T *t) {
    std::cout << "destroy_test class type " << endl;
}

template<>
void destroy_test<string, 1>(string *t) {
    std::cout << "destroy_test int 1 " << endl;
}

// #5 ，通过模板形参启用
template<class T,
        typename = Enable_if<std::is_array<T>::value> >
void destroy(T *t) // 注意，不修改函数签名
{
    for (std::size_t i = 0; i < std::extent<T>::value; ++i) {
        destroy((*t)[i]);
    }
}

//typename = Enable_if<!std::is_class<T>::value, int>, 其实是引入了一个匿名模版类型参数,由于其不能被推断（函数模版中），所以默认值会被计算，因此Enable_if生效
//所以如果定义如下的模版，则会报重定义错误
/*
template <typename T1, typename T2>
void des_test(T1* t){

}*/


template<typename T>
void des_test(T *) {
    cout << "des_test Enable" << endl;
}; //模版声明 1


template<typename T, typename = Enable_if<!std::is_class<T>::value, int> >
void des_test(T *t) {
    cout << "des_test general" << endl;
} // 模版声明2， 是否有效取决于Enable_if

template<>
void des_test<string>(string *t) {
    cout << "des_test for string" << endl;
} //模版特化时不能有两个模版声明可用,否则报二义性错误
/*
template<class T,
typename = std::enable_if_t<std::is_void<T>::value> >
void destroy(T* t){} // 错误：与 #5 拥有相同签名
*/

// A 的部分特化通过模板形参启用
template<class T, class Enable = void>
class A {
public:
    A() { cout << "this is not float type " << DeMangle(typeid(T).name()) << endl; }
}; // 初等模板

template<class T>
//class A<T, typename std::enable_if<std::is_floating_point<T>::value,T>::type> {
class A<T, typename std::enable_if<std::is_floating_point<T>::value, int>::type> {
public:
    A() { cout << "this is float type " << DeMangle(typeid(T).name()) << endl; }
}; // 为浮点类型特化


//常见错误是声明两个函数模板，而它们仅于其默认模板实参相异。这是无效的，
//因为这些函数声明被当做同一函数模板的再声明（默认模板实参不为函数模板等价所考虑）
/*
struct T1 {
enum { int_t,float_t } m_type;
template <typename Integer, typename = Enable_if <std::is_integral<Integer>::value> >
T1(Integer) : m_type(int_t) {}

template <typename Floating, typename = Enable_if<std::is_floating_point<Floating>::value> >
T1(Floating) : m_type(float_t) {} // error: cannot overload
};*/

//正确的方式是下面这样
struct T2 {
    enum {
        int_t, float_t
    } m_type;

    template<typename Integer, Enable_if<std::is_integral<Integer>::value, int> = 0>
    T2(Integer) : m_type(int_t) {}

    template<typename Floating, Enable_if<std::is_floating_point<Floating>::value, int> = 0>
    T2(Floating) : m_type(float_t) {} // OK
};


//函数模板 #1 与 #3 拥有不同签名，且为不同的模板。不过 #2 与 #4 尽管是不同函数模板的实例化，
//却于 Itanium C++ ABI 拥有相同的重整名（ _Z4funcI1XLi0EEvv ），这表示链接器会错误地认为它们是同一实体。

/*
// 第一个翻译单元
struct X {
enum { value1 = true, value2 = true };
};

template<class T, Enable_if<T::value1, int> = 0>
void func() {} // #1

template void func<X>(); // #2

// 第二个翻译单元


struct X {
enum { value1 = true, value2 = true };
};

template<class T, Enable_if<T::value2, int> = 0>
void func() {} // #3

template void func<X>(); //#4
*/

}//namespace enable
}//namespace tpl
#endif //CPPTEMPLATE_ENABLE_IF_H
