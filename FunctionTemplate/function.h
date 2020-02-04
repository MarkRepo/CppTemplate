//
// Created by markfqwu on 2020-01-28.
//

#ifndef CPPTEMPLATE_FUNCTION_H
#define CPPTEMPLATE_FUNCTION_H

#include <iostream>
#include <string>
#include <typeinfo>
#include <utility>
#include <vector>

#include "CommonTools/typeinfodemangle.h"

namespace tpl{
namespace function{

using std::string;
using std::cout;
using std::endl;
using std::pair;
using tpl::common::DeMangle;
using tpl::common::DeMangleToCout;
using std::vector;

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

//函数模版重载规则
//1 找到参与重载解析的所有函数模版特例化版本
//2 如果两个函数模版都可以调用，且其中一个比另一个更特殊，则接下来的步骤只考虑最特殊化的版本
//3 对前两个步骤后还留在候选集中的函数模版和所有候选普通函数一起进行重载解析。如果一个函数模版实参是通过模版实参推断确定的，则不能再对它进行提升、标准类型转化或用户自定义类型转化
//4 如果一个普通函数和一个特例化版本匹配的一样好，优先选择普通函数
//5 如果没有发现任何匹配，或者得到多个一样好的匹配，则是错误的

//SFINAE substituition failure is not an error 实参代入失败不是错误
template <typename Iter>
typename  Iter::value_type mean(Iter first, Iter last){
    cout << "in typename Iter::value_type mean(Iter first, Iter last"  << endl;
}  // 1 号

template <typename T>
T mean(T*, T*){
    cout << "in T mean(T*, T*)" << endl;
}    //2 号

void mean_test(std::vector<int>& v, int*p , int n){
    auto x = mean(v.begin(), v.end()); // 正确，调用1号
    auto y = mean(p, p+n);             // 正确，调用2号
}

//如果没有2号声明，mean(p, p+n) 会编译错误，因为int*没有value_type成员。
//当存在2号声明时，试图将mean(p, p+n)与1号匹配时，发生代入失败，它不是错误，只会导致模版被忽略，即模版的特例化版本不会成为候选。于是匹配2号版本

//如果一个模版特例化版本会导致类型错误，它会被认为是无意义的，从而不会加入候选集中。在此，只考虑声明，模版函数定义和类成员函数的定义除非真被使用，否则不在考虑范围之内
template <typename Iter>
Iter mean2(Iter first, Iter last){   // 1号

    cout << "in Iter mean2(Iter first, Iter last)" << endl;
    typename Iter::value_type v = *first;
    //...
}

template <typename T>   // 2号
T* mean2(T*, T*){

    cout << "in T* mean2(T*, T*)" << endl;
}

void mean2_test(std::vector<int>&v, int* p, int n){
    auto x = mean2(v.begin(), v.end()); // 正确， 调用1号
    auto y = mean2(p, p+n);  // 正确，调用2号
}

//假如没有2号声明，编译器会为mean2(p, p+n)选择1号版本，从而得到一个实例化错误

//重载和派生，重载解析规则保证函数模版能完美的和继承机制结合使用

template <typename T> class B{};
template <typename T> class D : public B<T> {};
template <typename T>
void tpl_f(B<T>* pb){
    cout << "in tpl_f  " << DeMangle(typeid(pb).name())<< endl;
}

void tpl_g(B<int>* pb, D<int>* pd){
    cout << "in tpl_g pb " << DeMangle(typeid(pb).name()) << endl;
    cout << "in tpl_g pd " << DeMangle(typeid(pd).name()) << endl;
    tpl_f(pb);
    tpl_f(pd); // D<int>* 到B<int>* 类型转换
}

//重载和非推断的参数
//对于在模版参数推断中未用到的函数实参，其处理方式与非模版函数实参完全相同，特别是，常用类型转换规则仍然有效。参考重载规则3
template <typename T, typename C>
T get_nth(C& c, int n){
    cout << "in get_nth" << endl;
    return c[n];
}

class Index{
public:
    Index(int i):i_(i){}
    operator int(){
        cout << "in operator int()" << endl;
        return i_;
    };
    int i_;
};

void tpl_f1(vector<int>& v, short s, Index i){
    int i1 = get_nth<int>(v, 2);
    int i2 = get_nth<int>(v, s);// short --> int
    int i3 = get_nth<int>(v, i);// Index --> int ,用户自定义类型转换

    cout << "i1 " << i1 << ",i2 " << i2 << ",i3 " << i3 << endl;
}

//使用using为模版定义别名，模版的一些参数可以固定
template <typename T>
class Alloc{};
template <typename T>
class MyAlloc{};
template <typename T, typename Allocator = Alloc<T> >
class MyVector{};
template<typename T>
using Vec = MyVector<T, MyAlloc<T>>;

//别名和原始模版等价，这意味着，当使用别名时，如果用到了模版特例化，就会得到特例化版本
template <int>
struct int_exact_traits{
    using type = int;
};

template <>
struct int_exact_traits<8>{
    using type = char;
};

template <>
struct int_exact_traits<16>{

    using type = short;
};

template <int N>
using int_exact = typename int_exact_traits<N>::type;


}//namespace function
}//namespace tpl



#endif //CPPTEMPLATE_FUNCTION_H
