//
// Created by markfqwu on 2020-02-08.
//

#ifndef CPPTEMPLATE_SPECIALIZATION_H
#define CPPTEMPLATE_SPECIALIZATION_H

#include <string>
#include <iostream>
#include <vector>

#include "CommonTools/typeinfodemangle.h"

using std::string;
using std::cout;
using std::endl;
using std::vector;
using tpl::common::DeMangle;

namespace tpl{
namespace specialization{

//模版可以接受参数
//1 "类型类型"的类型参数
//2 内置类型值参数
//3 "模版类型"的模版参数

//一个类型必需在作用域内且可访问，才能作为模版实参

//模版值参数可以是：
//1 整型常量表达式
//2 外部链接的对象或函数的指针或引用
//3 指向非重载成员的指针
//4 空指针

//一个指针必需具有&of或f的形式，才能作为模版实参，of是对象或函数的名字，f是函数的名字
//指向成员的指针必需具有&X::of形式，of是成员的名字
//字符串字面值常量不能作为模版实参
//整数模版实参必需是一个常量，同理，值模版参数在模版内部是一个常量

//在模版参数列表中，一个类型模版参数出现后即可当中一个类型来使用：
template <typename T, T default_value = T{}>
class Vec{};

//操作作为模版实参，将操作表示为一个类型实参而不是值实参，更灵活。
//1 值实参例子,这种方式不灵活，设计者必需决定是使用函数指针还是特定类型的函数对象进行比较，且依赖K的类型，很难提供默认的比较准则
template <typename K, typename V, bool(*cmp)(const K&, const K&)>
class Map{};

//2 类型实参例子，灵活，可以是函数指针，函数对象或是可以转化为函数指针的lambda表达式
template <typename K, typename V, typename Compare = std::less<K> >
class map{
public:
    map(){}
    map(Compare c):cmp{c}{
        cout << "c type is " << DeMangle(typeid(c).name()) << endl;
    }
    Compare cmp{}; //使用默认参数
};

using Cmp = bool(*)(const string&, const string&);
bool insensitive(const string& x , const string& y){return false;}

void Compare_test(){
    map<string, int> m1; //使用默认比较
    map<string, int, std::greater<string>> m2; //函数对象
    map<string, int, Cmp> m3{insensitive}; //函数指针
    map<string, int, Cmp> m4{[](const string& a, const string& b){return a>b;}}; //lambda表达式转化为函数指针

    //lambda不能转化为函数对象类型
    auto cmp = [](const string& a, const string& b){return a>b;};
    map<string, int, decltype(cmp)> m5{cmp};

    cout << "cmp type is " << DeMangle(typeid(cmp).name()) << endl;
    cout << "m4 type is " << DeMangle(typeid(m4).name()) << endl;
    cout << "m5 type is " << DeMangle(typeid(m5).name()) << endl;
}

//模版作为实参
//为了将一个模版指定为模版参数，必须指定其所需的实参用于特例化。使用模版参数通常是希望用多种实参类型（如T，T*）对其进行实例化
//只有类模版可以作为模版实参
template <typename T, template <typename> class C>
class Xrefd{
    C<T> mems;
    C<T*> refs;
};

template <typename T>
using My_vec = vector<T>;

class Entry{};

template <typename T>
class MyContainer{};

class Record{};

void tplarg_test(){
    Xrefd<Entry, My_vec> x1;
    Xrefd<Record, MyContainer> x2;
}

//默认模版实参
//1。 只有当真正使用默认模版实参时，编译器才会对其进行语义检查
//2。 只能对尾部模版参数指定和提供默认实参

//默认函数模版实参
//只有当一个函数模版实参类型无法推断或没有默认实参时，才需要显示指定
//如果所有函数模版参数都有默认实参，则<>可以省略（与函数模版特例化一样）

//特例化
template <typename T>
class Vector{
public:
    Vector();
    explicit Vector(int);
    T& elem(int i){return v[i];}
    T& operator[](int i);
    void swap(Vector&);

private:
    T* v;
    int sz;
};

//通过特例化让所有指针的容器共享一个特殊的实现，避免代码膨胀
template <>
class Vector<void*>{
public:
    explicit Vector(int i){}
    void*& elem(int i){return p[i];}
    void*& operator[](int i);
private:
    void** p;
};

//<T*>指出这个版本跟用于所有指针类型
//模式（此例中为T*）中包含模版参数的特例化版本成为部分特例化，此时模版参数是从模式中推断出的
template <typename T>
class Vector<T*> : private Vector<void*>{
public:
    using Base = Vector<void*>;
    Vector(){}
    explicit Vector(int i) : Base{i}{}
    T*& elem(int i){return reinterpret_cast<T*&>(Base::elem(i));}
    T*& operator[](int i){return reinterpret_cast<T*&>(Base::operator[](i));}
};

//接口特例化：在特例化实现中接口有所不同
//实现特例化：特例化的实现不一样
template <typename T, int N>
class Matrix;

template <typename T>
class Matrix<T,0>{
    T val;
};

template <typename T>
class Matrix<T,1>{
    T val;
    int sz;
};

template <typename T>
class Matrix<T,2>{
    T* elem;
    int dim1;
    int dim2;
};

//主模版：指最通用模版定义，为所有特例化版本定义了接口，即主模版用来确定某个模版的使用是否合法，并参与重载解析
//主模版必须在任何特例化之前声明，它指出在使用模版和特例化时需提供哪些模版参数
//在使用时定义必需时可见的
//显示特例化一个模版意味着编译器不再为此特例化版本生成定义
//模版的所有特例化版本都应声明在与主模版相同的名字空间中
template <typename T>
class List{
public:
    List(){cout<< "General List is used" << endl;}
};

//此处使用主模版
List<int*> li;

template <typename T>
class List<T*>{
public:
    List(){cout << "pointer List is used"<<endl;}
};

//由于List<int*> 已使用主模版实例化，故这里也是使用主模版
List<int*> li2;
//此处使用特例化版本
List<double*> ld2;

//一个特例化版本比另一个更特殊是指与其特例化模式匹配的所有实参列表也都与另一个版本的特例化模式匹配，但反之不成立。最特殊的优先被采纳。

//函数模版特例化
//1 c++仅支持完整特例化，可用重载代替部分特例化。

template <typename T>
bool Less(T a , T b){
    cout << "Less, T type is " <<DeMangle(typeid(T).name())<<endl;
    return a < b;
}
template <typename T>
bool Less1(T a , T b){
    cout << "Less1, T type is " <<DeMangle(typeid(T).name())<<endl;
    return a < b;
}
template <typename T>
bool Less2(T a , T b){
    cout << "Less2, T type is " <<DeMangle(typeid(T).name())<<endl;
    return a < b;
}
//为const char* 类型特例化
template <>
bool Less<const char*>(const char* a , const char* b){
    cout << "Less for const char*" << endl;
    return strcmp(a,b) < 0;
}

//模版实参可从函数实参推断出来，可简化如下
template<>
bool Less1<>(const char* a, const char* b){
    cout << "Less1 for const char*" << endl;
    return strcmp(a,b)<0;
}
//给定前缀tempalte<>, 第二个<>就是多余的了
template<>
bool Less2(const char* a, const char* b){
    cout << "Less2 for const char*" << endl;
    return strcmp(a,b)<0;
}

//重载
bool Less(const char*a, const char* b){
    cout << "Less Overload" <<endl;
    return strcmp(a,b)<0;
}
//函数模版特例化的一个用途时在无实参的函数中进行选择
template <typename T>
T max_value();

template <>
int max_value<int>(){
    cout << "int max value"<<endl;
    return 1;
}

template<>
char max_value<char>(){
    cout << "char max value"<<endl;
    return 'a';
}
} //namespace specialization
} // namespace tpl

#endif //CPPTEMPLATE_SPECIALIZATION_H
