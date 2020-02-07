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
template <typename T>
struct Error{
    Error(){cout << "this is Error<T> type" << endl;}
};

template <typename T>
using GetUnsignedIntegerT  = typename std::conditional<std::is_integral<T>::value, typename std::make_unsigned<T>::type, Error<T>>::type;

template <typename T>
using Make_unsigned =typename std::make_unsigned<T>::type;

template <typename T>
using Make_Error = Error<T>;

//此时可以使用显示的，面向实现的::type风格，或者引入一个类型函数Delay, 来将类型函数的求值延迟到使用时：
template <template<typename...>class F, typename... Args>
using Delay = F<Args...>;

template <typename T>
using GetUnsignedIntegerT_delay = typename std::conditional< std::is_integral<T>::value, Delay<Make_unsigned, T>, Error<T> >::type;

void GetUnsignedIntegerT_test(){
    GetUnsignedIntegerT<int> a;
    GetUnsignedIntegerT_delay<int> b;

    //由于不存在std::make_unsigned<std::string>::type, 所以这里编译错误
    //GetUnsignedIntegerT<std::string> c{};
    //同上，编译错误
    //GetUnsignedIntegerT_delay<std::string> d;  //编译错误。。。
    cout << "a type " << DeMangle(typeid(a).name()) << endl;
    cout << "b type " << DeMangle(typeid(b).name()) << endl;
    //cout << "c type " << DeMangle(typeid(c).name()) << endl;
    //cout << "d type " << DeMangle(typeid(d).name()) << endl;
}

//类型谓词，参数是类型且返回bool值的函数，如
template <typename T>
void my_copy(T* p, const T* q, int n){
    if(std::is_pod<T>::value) {
        cout << "pod copy " << DeMangle(typeid(T).name()) << endl;
        memcpy(p, q, sizeof(T) * n);
    }
    else {
        cout << "not pod copy " <<DeMangle(typeid(T).name())<< endl;
        for (int i = 0; i != n; i++)
            p[i] = q[i];
    }
}

template <bool B, typename T, typename F>
using Conditional = typename std::conditional<B,T,F>::type;

template <typename T>
void do_something(){
    //is_pod<T>() 被解释为一个无参，返回is_pod<T>的函数类型
    //Conditional<std::is_pod<T>(), On_heap<T>, Scoped<T>> x; //错误，is_pod<T>()是一个函数类型，而非bool值
}

//由于上面的表达方式在语言技术上的问题，因此常用如下简化的习惯表达方式
template <typename T>
constexpr bool Is_pod(){
    return std::is_pod<T>::value;
}

void pod_copy_test(){
    int a_i[] = {1,2,3,4};
    int b_i[4] = {0};
    std::string a_s[] = {"a", "b", "c", "d"};
    std::string b_s[4];
    my_copy(b_i, a_i, 4);
    my_copy(b_s, a_s, 4);
    for(auto i : b_i)
        cout << i << " ";
    cout << endl;
    for(auto s : b_s)
        cout << s << " ";
    cout << endl;
}

//可以自定义新的类型谓词
template <typename T>
constexpr bool Is_big(){
    return 100 < sizeof(T);
}

template <typename T>
using Big_obj_holder = Conditional< Is_big<T>(), On_heap<T>, Scoped<T>>;

void big_obj_holder_test(){
    Big_obj_holder<double> a;
    *a = 7.7;
}

//萃取被用来关联属性与类型，可以将萃取理解为返回很多结果的类型函数或一组类型函数，如迭代器的属性由iterator_traits定义
template <typename Iterator>
struct my_iterator_traits{
    using difference_type = typename Iterator::difference_type;
    using value_type = typename Iterator::value_type;
    using pointer = typename Iterator::pointer;
    using reference = typename Iterator::reference;
    using iterator_category = typename Iterator::iterator_category;
};

//基于此定义一些简单的类型函数
template <typename T>
using Value_type = typename my_iterator_traits<T>::value_type;

template <typename T>
using Difference_type = typename my_iterator_traits<T>::difference_type;

template <typename T>
using Iterator_category = typename my_iterator_traits<T>::iterator_category;

//可以这样使用
template <typename Iter>
Iter search(Iter p, Iter q, Value_type<Iter> val){
    Difference_type<Iter> m = q - p ;
    //...
}

//考虑在不了解萃取和任何类型函数的情况下编写search
template <typename Iter, typename Val>
Iter search(Iter p, Iter q, Val val){
    auto x = *p;
    auto m = q-p;

    using value_type = decltype(*p); //类型推断
    using difference_type = decltype(q-p); //类型推断
}

//控制结构
//1 选择
//1.1 在两个类型中选择， 如std::conditional
template <bool B,typename T, typename F>
struct my_conditional{
    using type = T;
};

//false的特例化版本，特例化被用来分离一般情况和特殊情况
template <typename T, typename F>
struct my_conditional<false, T, F>{
    using type = F;
};

template <bool B, typename T, typename F>
using MyConditional = typename my_conditional<B,T,F>::type;

//考虑运行时进行类型选择是否可取，即使用if或?:运算符

struct Square{
    constexpr int operator()(int i) const {return i*i;}
};

struct Cube{
    constexpr int operator()(int i) const {return i*i*i;}
};

template <typename T>
constexpr bool My_cond(){
    return sizeof(T) <= 8;
}

template <typename T>
int my_conditional_test(int i){
    int a = MyConditional<(My_cond<T>()), Square, Cube>{}(i);

    //错误，声明不能作为if语句分支的唯一语句
    if(My_cond<T>())
        using Type = Square;
    else
        using Type = Cube;

    //使用?:
    //auto b = (My_cond<T>()?Square:Cube){}(i); //错误，Square{}(10)和Cube{}()不产生类型，也不是相容类型的值
    //auto c = (My_cond<T>()?Square{}:Cube{})(i);//错误，?:运算符不允许运算对象不相容
    My_cond<T>()?Square{}(10):Cube{}(i); //正确
    cout << "T is " << DeMangle(typeid(T).name()) <<", sizeof(T) == "<< sizeof(T) <<", i = " << i << ", and result is " << a << endl;
    return a;
}

//1.2 在多个类型中选择
//1.2.1 第一种方式,使用特例化，缺点：个数限制
class Nil{};
template <int I, typename T1 = Nil, typename T2 = Nil, typename T3 = Nil>
struct select1;

template <typename T1, typename T2, typename T3>
struct select1<0, T1, T2, T3>{
    using type = T1;
};

template <typename T1, typename T2, typename T3>
struct select1<1, T1, T2, T3>{
    using type = T2;
};

template <typename T1, typename T2, typename T3>
struct select1<2, T1, T2, T3>{
    using type = T3;
};

template <int I, typename T1, typename T2, typename T3>
using Select1 = typename select1<I, T1, T2, T3>::type;

//1.2.2 第二种方式，使用可变参数模版
//递归调用，直至到达一个对应终止条件的特例化版本
template <unsigned N, typename... Cases>
struct select;

template <unsigned N, typename T, typename... Cases>
struct select<N, T, Cases...> : public select<N-1, Cases...>{};

template <typename T, typename... Cases>
struct select<0, T, Cases...>{
    using type = T;
};

template <unsigned N, typename... Cases>
using Select = typename select<N, Cases...>::type;

//使用递归实现编译时迭代，在编译时计算值
template <int N>
constexpr int fac(){
    return N*fac<N-1>();
}

template <>
constexpr int fac<1>(){
    return 1;
}

//使用类的递归处理更复杂的状态或更精细的参数化
template <int N>
struct Fac{
    static const int value = N*Fac<N-1>::value;
};

template <>
struct Fac<1>{
    static const int value = 1;
};
//优先选择系统化的技术，如特例化以及别名，而不是宏；为了实现编译时计算，优先选择constexpr函数而不是模版

//条件定义，Enable_if
//std::enable_if<B, T> 如果B为true，结果为第二个参数，如果为false，则其所在函数声明会被完全忽略。

template <bool B, typename T = void>
struct my_enable_if{
    using type = T;
};

template<typename T>
struct my_enable_if<false, T>{};

template <bool B, typename T = void>
using Enable_if = typename my_enable_if<B, T>::type;

template <typename T>
constexpr bool Is_class(){
    return std::is_class<T>::value;
}

template <typename T>
class Smart_pointer_base{

public:
    Smart_pointer_base(T* pt) :pt_(pt){}
    ~Smart_pointer_base(){if(pt_)delete pt_;}
    T& operator*(){return *pt_;}

    //Enable_if<Is_class<T>(), T>* operator->(){return pt_;}//实现处1，这种非模版声明方式导致Enable_if不能利用SFINA特性,只能在条件为true时使用
protected:
    T* pt_;
};

//类模版继承的属性需要用this访问，否则会报未声明错误
template <typename T, typename Enable = void>
class Smart_pointer : public Smart_pointer_base<T>{
public:
    Smart_pointer(T* pt):Smart_pointer_base<T>(pt){cout << "Smart_pointer not class, T type is "<< DeMangle(typeid(T).name()) << endl;}
};

template <typename T>
class Smart_pointer<T, Enable_if<Is_class<T>()>> : public Smart_pointer_base<T>{
public:
    Smart_pointer(T* pt):Smart_pointer_base<T>(pt){cout << "Smart_pointer is class, T type is "<< DeMangle(typeid(T).name()) << endl;}
    T* operator->(){cout << "get real pointer from Smart pointer" << endl; return this->pt_;}//实现处2
};

//非模版类继承的属性可直接访问
class Base{
public:
    int * pt_;
};

class Derived :public Base{
public:
    int* get(){return pt_;}
};

//Enable_if技术用于模版函数的实例
template<typename T>
class MyVector{
public:
    MyVector(size_t n, const T& val){
        cout << "MyVector use size_t and val version" << endl;
    }

    //MyVector<int> v(10,20)， 会使用该版本， 因为10到size_t需要类型转换
    template <typename Iter>
    MyVector(Iter b, Iter e){
        cout << "MyVector use Iter version" << endl;
    }
};

//Enable_if 用作默认模版实参
template <typename T>
class MyVectorEnable{
public:
    //MyVectorEnable<int> v(10,20)会使用该版本
    MyVectorEnable(size_t n, const T& val){
        cout << "MyVectorEnable use size_t and val version" << endl;
    }

    template <typename Iter, typename = Enable_if<Is_class<Iter>(), Iter> >
    MyVectorEnable(Iter b, Iter e){
        cout << "MyVectorEnable use Iter version" << endl;
    }
};

//Enable_if用作构造函数的实参类型
template <typename T>
class MyVectorEnableArg{
public:
    //MyVectorEnableArg<int> v(10,20)使用该版本
    MyVectorEnableArg(size_t n, const T& val){
        cout << "MyVectorEnableArg use size_t and val version" << endl;
    }

    template <typename Iter>
    MyVectorEnableArg(Enable_if<Is_class<Iter>(), Iter> b, Iter e){
        cout << "MyVectorEnableArg use Iter version" << endl;
    }
};

//问题: 若x的类型为X， 是否能调用f(x)?
//Has_f的实现:
struct substitution_failure{}; //表示声明失败

//std::true_type::value == true;
//std::false_type::value == false;
template <typename T>
struct substitution_succeeded : std::true_type{};

template <>
struct substitution_succeeded<substitution_failure> : std::false_type{};

//对所有类型都提供函数f，用于Has_f的测试
template <typename T>
void f(T){
    cout << "enter func f when Has_f test , T is " << DeMangle(typeid(T).name()) << endl;
}

//f(x)的返回类型不是substitution_failure时，下面的技术才能生效
template <typename T>
struct get_f_result{
private:
    //decltype不会对其运算对象求值
    template <typename X>
    static auto check(X const& x)-> decltype(f(x));// 可以调用f(x)时，check的声明才能编译通过, check的返回类型与f(x)相同
    static substitution_failure check(...); //不能调用f(x)时返回substitution_failure

public:
    using type = decltype(check(std::declval<T>()));
};

//如果get_f_result<T>生成一个恰当的类型（调用f的返回类型）， 则has_f::value 为true；
//若get_f_result<T> 编译失败，他会返回substitution_failure,因而has_f::value为false
template <typename T>
struct has_f : substitution_succeeded<typename get_f_result<T>::type>{};

//掌握了Has_f定义所使用的技术后，就可以为能想到的任何操作或成员foo定义Has_foo了
template <typename T>
constexpr bool Has_f(){
    return has_f<T>::value;
}

//使用Has_f, X<T>具有成员use_f()当且仅当对类型为T的t值可以调用f(t)
template <typename T>
class X{
public:
    Enable_if <Has_f<T>()> use_f(const T& t){
        f(t);
    }
};

}// namespace meta
}// namespace tpl
#endif //CPPTEMPLATE_META_H
