//
// Created by markfqwu on 2020-02-07.
//

#ifndef CPPTEMPLATE_UNIT_H
#define CPPTEMPLATE_UNIT_H

#include "MetaProgramming/meta.h"

#include <string>
#include <iostream>

using std::string;
using std::ostream;

namespace tpl{
namespace unit{

//使用元编程实现一个单位系统的计算

template <int M, int K, int S>
struct Unit{
    enum{m=M, kg=K, s=S};
};

using M = Unit<1,0,0>; //米
using Kg = Unit<0,1,0>; //千克
using S = Unit<0,0,1>; //秒
using MpS = Unit<1,0,-1>;// m/s
using MpS2 = Unit<1,0,-2>; //米每平方秒

//两个量相乘，单位相加
template <typename U1, typename U2>
struct Uplus{
    using type = Unit<U1::m+U2::m, U1::kg+U2::kg, U1::s+U2::s>;
};

template <typename U1, typename U2>
using Unit_plus = typename Uplus<U1,U2>::type ;

//两个量相除，单位相减
template <typename U1, typename U2>
struct Uminus{
    using type = Unit<U1::m-U2::m, U1::kg-U2::kg, U1::s-U2::s>;
};

template <typename U1, typename U2>
using Unit_minus = typename Uminus<U1, U2>::type;

//一个Quatity是一个关联Unit的值
template<typename U = double>
struct Quantity{
    long double val;
    explicit constexpr Quantity(long double d):val(d){}
};

template <typename U>
Quantity<U> operator+(Quantity<U> x, Quantity<U> y){
    return Quantity<U>{x.val + y.val};
}

template <typename U>
Quantity<U> operator-(Quantity<U> x, Quantity<U> y){
    return Quantity<U>{x.val - y.val};
}

template <typename U1, typename U2>
Quantity<Unit_plus<U1,U2>> operator*(Quantity<U1> x, Quantity<U2> y){
    return Quantity<Unit_plus<U1,U2>>{x.val*y.val};
}

template <typename U1, typename U2>
Quantity<Unit_minus<U1,U2>> operator/(Quantity<U1> x, Quantity<U2> y){
    return Quantity<Unit_minus<U1,U2>>{x.val/y.val};
}

//与无量钢体操作
template <typename U>
Quantity<U> operator*(Quantity<U> x, double y){
    return Quantity<U>{x.val*y};
}

template <typename U>
Quantity<U> operator/(Quantity<U> x, double y){
    return Quantity<U>{x.val/y};
}

template <typename U>
Quantity<U> operator*(double x, Quantity<U> y){
    return Quantity<U>{x*y.val};
}

//为Quantity类型引入用户自定义字面值常量
//模版字面值常量运算符将其参数作为模版参数而不是函数参数，见后面的3进制字面值常量例子
//整型字面值常量：可用于接受unsigned long long 和 const char* 参数的字面值常量运算符， 也可用于模版字面值常量运算符
//浮点型字面值常量：可用于接受long double 和 const char* 参数的字面值常量运算符， 也可用于模版字面值常量运算符
//字符串字面值常量： 可用于接受(const char*, size_t)参数对的字面值常量运算符
//字符字面值常量：可用于接受char, wchar_t, char16_t, char32_t等字符类型参数的字面值常量运算符
constexpr Quantity<M> operator""_m(long double d){return Quantity<M>{d};}
constexpr Quantity<Kg> operator""_kg(long double d){return Quantity<Kg>{d};}
constexpr Quantity<S> operator""_s(long double d){return Quantity<S>{d};}

//更多常用单位用户自定义字面值常量
constexpr Quantity<M> operator""_km(long double d){return Quantity<M>{d*1000};}
constexpr Quantity<Kg> operator""_g(long double d){return Quantity<Kg>{d/1000};}
constexpr Quantity<Kg> operator""_mg(long double d){return Quantity<Kg>{d/1000000};}
constexpr Quantity<S> operator""_ms(long double d){return Quantity<S>{d/1000};}
constexpr Quantity<S> operator""_us(long double d){return Quantity<S>{d/1000000};}
constexpr Quantity<S> operator""_ns(long double d){return Quantity<S>{d/1000000000};}

template <typename U>
Quantity<Unit_plus<U,U>> square(Quantity<U> x){
    return Quantity<Unit_plus<U,U>>{x.val*x.val};
}

template <typename U>
bool operator==(Quantity<U> x, Quantity<U> y){
    return x.val == y.val;
}

template <typename U>
bool operator!=(Quantity<U> x, Quantity<U> y){
    return x.val != y.val;
}

string suffix(int u, const char* x){//辅助函数
    string suf;
    if(u){
        suf += x;
        if(1<u) suf+='0'+x;
        if(u<0){
            suf += '-';
            suf += '0'-u;
        }
    }
    return suf;
}

template <typename U>
ostream& operator<<(ostream& os, Quantity<U> v){
    return os << v.val << suffix(U::m, "m") << suffix(U::kg, "kg") << suffix(U::s, "s");
}

//3进制字面值常量实现
constexpr int ipow(int x, int n){ //对n>=0, 求x的n次方
    return (n>0)?x*ipow(x, n-1):1;
}

template <char c>
constexpr int b3_helper(){
    static_assert(c<'3', "not a ternay digit");
    return c-'0';
}

template <char c, char... tail>
constexpr tpl::meta::Enable_if<(sizeof...(tail)>0), int>
b3_helper(){
    static_assert(c<'3', "not a ternay digit");
    return ipow(3, sizeof...(tail))*(c-'0') + b3_helper<tail...>();
}

template <char... chars>
constexpr int operator""_b3(){
    return b3_helper<chars...>();
}

}   //namespace unit
}   //namespace tpl

#endif //CPPTEMPLATE_UNIT_H
