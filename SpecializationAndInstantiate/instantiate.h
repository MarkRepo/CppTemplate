//
// Created by markfqwu on 2020-02-08.
//

#ifndef CPPTEMPLATE_INSTANTIATE_H
#define CPPTEMPLATE_INSTANTIATE_H

#include <iostream>
#include "MyVector.h"

namespace tpl {
    namespace ext {
        //显示实例化确定了单一的实例化点，从而其他实例化点被忽略，可以利用这一点将显示实例化置于共享库中
        extern template class tpl::ext::MyVector<int>;
    }
}
#include <vector>

using std::vector;
using tpl::ext::MyVector;
using std::cout;
using std::endl;

namespace tpl{
namespace instantiate{

//编译器从下列来源组合代码信息
//1 模版定义及其词法环境
//2 模版实参及其词法环境
//3 模版使用环境

//何时需要实例化
//生成的类和函数称为特例化，分为生成特例化和显示特例化
//只有在需要类定义时才必需生成类模版的特例化版本，特别的，如果只是为了声明类的指针，是不需要实际的类定义的。
//模版的使用之处定义了一个实例化点
template <typename T>
class Link{
    Link* suc; //还不需要Link的定义
};

Link<int>* pli; //不实例化Link<int>
Link<int> li; //实例化Link<int>

//对于函数模版，只有当他真正被使用时，才需要一个函数实现来实例化它。被使用的含义时"被调用或被获取地址"
//特别的，实例化一个类模版并不意味要实例化它的所有成员函数。

// 手工控制实例化
//目的：
//1 通过消除冗余的重复实例化代码来优化编译和链接过程
//2 准确掌握哪些实例化点被使用，从而消除复杂名字绑定上下文带来的意外

//一个显示实例化请求在语法上就是一个特例化声明加上关键子template前缀（后面没有<），区别于模版声明开头是template<
//template 后需要接一个完整声明，而不仅仅是名字
template <typename T, typename S>
T convert(S s){return T{};}

template <typename T>
class Vector{
public:
    T&operator[](int i){return v[i];}

private:
    T* v;
    int sz;
};

//显示实例化一个类模版时，他的所有成员函数也同时被实例化
template class Vector<int>; //类
template double& Vector<double>::operator[](int);//成员函数
template int convert<int, double>(double); //非成员函数

//与模版函数调用类似，可以忽略可以从函数实参推断出得模版实参,如
template int convert<int>(float);

//作为显示实例化的补充，C++还提供了显示不实例化的请求（通常称为外部模版，extern template）
//用途：在某个编译单元对一个特例化版本进行显示实例化，在其他编译单元使用此版本时，使用extern template

void foo(MyVector<int>& v){

}

//名字绑定
//为模版显示或隐式使用的每个名字寻找其声明的过程称为名字绑定，模版实例化涉及的三个上下文：
//1 模版定义上下文
//2 实参类型声明的上下文
//3 模版使用上下文

//模版定义中使用的名字可以分为两类：
//1 依赖性名字： 即，依赖于模版参数的名字，这类名字在实例化点完成绑定
//2 非依赖性名字：不依赖于模版参数的名字，这类名字在模版的定义点完成绑定
//无论是依赖性名字还是非依赖性名字，都必须位于其使用点的作用域中，或是在"实参依赖"查找中能找到

//基本原则：
//1 模版定义应尽量降低对非局部信息的依赖，尽可能的令模版定义是自包含的
//2 当需要一些非局部实体时，优选具名的名字空间而非全局作用域

//依赖性名字
//一个函数调用依赖一个模版参数，当且仅当满足下列条件
//1 根据类型推断规则，函数实参的类型依赖于一个模版参数T，如f(T(1)), f(t), f(g(t))及f(&t),假定t的类型是T
//2 根据类型推断规则，函数有一个参数依赖于T，例如，f(T), f(list<T>), f(const T*)
//大体上，如果被调函数的实参或形参明显依赖于模版参数，则函数名是依赖性名字

class Quad{};

Quad g(Quad){return Quad{};}

template <typename T>
T f(T a){
    return g(a); // a 是一个依赖性名字，因此g也是
}

auto z = f(Quad{});
//如果函数调用恰巧有一个实参与实际的模版参数类型匹配，则不是依赖性的

Quad gg(Quad){return Quad{};}
template <typename T>
T ff(T a){
    return gg(Quad{});//gg不是依赖性的
}

auto zz = ff(Quad{});

//默认情况下，编译器假定依赖性名字不是类型名，因此，为了使依赖性名字可以是一个类型，必须用关键字typename显示说明
template <typename C>
void fct(C & c){
    //C::value_type v1 = c[7]; //语法错误，value_type不是类型名
    typename C::value_type v2 = c[9];
    auto V3 = c[11]; //编译期推断
}

//命名.(点)，->或::后面的成员模版需要使用关键字template
class Pool{
public:
    template <typename T>
    T* get(){return new T;}
    template <typename T>
    void release(T*);
};

template <typename Alloc>
void f1(Alloc& all){
    //int* p1 = all.get<int>();//错误，编译器假定get是非模版
    all.template get<int>();//正确
    cout << "f1 template get" << endl;
}

void user(Pool& pool){
    f1(pool);
}

//定义点绑定
//当编译器遇到一个模版定义时，会判断哪些名字是依赖性的，如果名字是依赖性的，将查找声明的工作推迟到实例化时
//编译器将非依赖名字当作模版外的名字一样处理，因此，在定义点位置，这种名字必需在作用域中
//如果找到了名字的声明，则编译器会使用这个声明，即使随后可能发现"更好"的声明也是如此

void before_g(double){cout << "before_g double" <<endl;}
void before_g2(double){cout << "before_g2 double"<<endl;}

template <typename T>
int at_ff(T a){
    before_g2(2); //调用before_g2(double);
    //after_g3(2);//g3 不在作用域
    before_g(2);//调用before_g(double), before_g(int)不在作用域
    return 0;
}

void before_g(int){cout << "before_g int" << endl;}
void after_g3(int){cout << "after_g3 int"<<endl;}

//实例化点绑定
//对一个函数模版而言，实例化点位于包含模版使用的最近的全局作用域或名字空间作用域中，恰好在包含此次使用的声明之后
void ggg_test(int){cout << "ggg_test int " <<endl;}
template <typename T>
void fff(T a){
    ggg_test(a);
}
void hhh(int i){
    extern void ggg_test(double);
    fff(i);
}
//fff<int>的实例化点

//fff<int>的实例化点在hhh(int) 之外是很重要的，它保证了hhh中调用的ggg_test是全局的ggg_test(int)而非局部的ggg_test(double)
//模版定义中一个未限定的名字永远也不应该绑定到一个局部名字上

//为了允许递归调用，函数模版的实例化点位于实例化它的声明之后：
void ggg_test2(int){cout << "ggg_test2 int"<<endl;}

void hhh2(int i); //hhh2 声明
template <typename T>
void fff2(T a){
    ggg_test2(a); //ggg_test2在实例化点绑定
    //如果没有hhh2前向声明，报error: call to function 'hhh2' that is neither visible in the template definition nor found by argument-dependent lookup
    if(a)hhh2(a-1);//hhh2在实例化点绑定；
}

void hhh2(int i){
    extern void ggg_test2(double);
    fff2(i);
}
//fff2<int> 的实例化点
//此例中，实例化点必须位于hhh2的定义之后，否则递归调用hhh2(a-1)就无法处理了，因为此时hhh2不在作用域

//对于一个模版类或类成员而言，实例化点恰好位于包含其使用的声明之前
template <typename T>
class Container{
private:
        vector<T> v;
public:
        void sort(){}
};

//Container<int>的实例化点在这里
void fff3(){
    Container<int> c; //使用点
    c.sort();
}

//局部名字可以作为模版实参，不能出现在模版定义中
void fff4(){
    struct S{int a, b;};
    vector<S> vs;
}

//多实例化点
//如果选择不同的实例化点可能导致两种不同的含义，则程序是非法的
namespace N {
    class X{};
    int ng(X,int){cout << "int version"<<endl;}
}

template <typename T>
int m_ff(T t, double d){
    ng(t, d);
    return 1;
}

auto X1 = m_ff(N::X{}, 1.1); //使用ng(X,double)

namespace N{
    int ng(X, double){cout << "double version" << endl;}
}

auto X2 = m_ff(N::X{}, 2.2);//使用ng(X,double)

//实参依赖查找机制：当函数被调用时，即使其声明不在当前作用域中，只要他是在某个实参所在的名字空间中声明的，编译器就能找到它
//编译器完成依赖性名字的绑定：
//1。模版定义点所处作用域的名字
//2。依赖性调用的一个实参的名字空间中的名字

namespace M{

    class A{};
    char df(A){cout << "A type version" << endl;}
}

char df(int){cout << "int type version" << endl;}

template <typename T>
char dg(T t){
    return df(t);
}

char df(double){cout << "double type version" << endl;}

char c1 = dg(M::A{});
char c2 = dg(2);
char c3 = dg(2.1);// df(int）被调用，df(double)不在作用域


//来自基类的名字
void bg(int) {cout << "::bg(int) version" << endl;}

struct BB{
    void bg(int){cout << "BB::bg int version"<<endl;}
    void bh(int){cout << "BB::bh int version"<<endl;}
};

template <typename T>
class BX : public T{
public:
    void bf(){
        bg(2);//调用::bg(int),因为bg(2)不依赖模版参数，会在定义点绑定
    }
};

void bh(BX<BB> x){
    x.bf();
}

//将依赖性名字纳入考虑的方式：
//1 用依赖性类型限定，如T::g
//2 声明一个名字指向此类的一个对象（this->g）
//3 用using声明将名字引入作用域

void b1g(int){cout << "::b1g version" << endl;}
void b1g2(int){cout << "::b1g2 version" << endl;}

struct B1B{

    using Type = int;
    void b1g(int){cout << "B1B::b1g version" << endl;}
    void b1g2(int){cout << "B1B::b1g2 version" << endl;}
};

template <typename T>
class B1X : public T{
public:
    typename T::Type m;
    using T::b1g2;

    void b1f(){
        this->b1g(2); //调用T::B1g
        b1g(2); //调用::B1g(int)
        b1g2(2);//调用T::B1g2
    }
};

void b1h(B1X<B1B> x){
    x.b1f();
}

template <typename T>
class Matrix_base{
    int size()const {return sz;}

protected:
    int sz;
    T* elem;
};

template <typename T, int N>
class Matrix : public Matrix_base<T>{
    T* data(){
        return this->elem;//类层次被模版化，需要显示限定对模版的依赖性基类成员的访问
    }
};

}   //namespace instantiate
}   //namespace tpl

#endif //CPPTEMPLATE_INSTANTIATE_H
