//
// Created by markfqwu on 2020-01-19.
//

#ifndef CPPTEMPLATE_MEMBER_H
#define CPPTEMPLATE_MEMBER_H

#include <iostream>
#include <typeinfo>
#include "CommonTools/typeinfodemangle.h"

namespace tpl {
namespace member {

using std::cout;
using std::endl;
using tpl::common::DeMangle;
using tpl::common::DeMangleToCout;

//0. 类模版成员的规则与普通类成员的规则是一样的。
//1. 数据成员
//非static数据成员可以是const，但不能是constexpr的

//2. 成员函数
//非static成员函数的定义可以在类模版内部或外部
//类模版的成员函数可以是virtual，但是成员模版不能是virtual的

//3. 成员类型别名
//可以使用using或typedef定义向类模版引入成员类型别名，方便类外访问。

//4. static 成员
//static 成员只有真正被使用时才需要定义
//类外定义的static成员在整个程序中只能有唯一一个定义
//const或constexpr static的字面值常量类型数据成员可以在类内初始化

//5. 成员类型
//可以是一个类或者一个枚举，成员枚举可以类外定义，但是类内声明必须给出基础类型：
template<typename T>
struct MT {
    enum E1 {
        a, b
    };
    //enum E2; //错误，基础类型未知
    enum class E3;
    enum E4 : char;
    struct C1 {/*...*/};
    struct C2;
};

template<typename T>
enum class MT<T>::E3 {
    a, b
}; //必需

template<typename T>
enum MT<T>::E4 : char {
    x, y
}; //必需

template<typename T>
struct MT<T>::C2 {
}; //必需

//6. 成员模版
//模版成员，可以增加类型表示的控制度和灵活性：
template <typename Scalar>
class complex{
    Scalar re, im;
public:
    complex() :re{}, im{}{} //{} 防止窄化转换
    template <typename T>
    complex(T rr,T ii =0): re{rr}, im{ii} {}

    complex(const complex& ) = default;
    template <typename T>
    complex(const complex<T>& c) : re{c.real()}, im{c.imag()} {}

    Scalar real() const {return re;}
    Scalar imag() const {return im;}
};
//成员模版不能是virtual，因为每当有新的实例化时，连接器需要向虚函数表里添加一项，这回增加连接器的复杂性

//对于类模版的成员，必需考虑参数化对成员类型是否恰当。一个模版成员依赖于所有模版实参，当成员的行为实际上并未使用所有模版实参时，这种依赖会产生副作用。
//如下例子所示，Link是BadList的实现细节，由于BadList<double, My_Allocator>::Link 和 BadList<double, Your_Allocator>::Link是不同的类型，会导致代码膨胀。
template <typename T, typename Allocator>
class BadList{
private:
    struct Link{
        T val;
        Link* succ;
        Link* prev;
    };
};
//考虑Link不是成员的设计
template <typename T, typename Allocator>
class List;

//将Link的所有成员都声明为private，并授予List访问权限
template <typename T>
class Link{
    template <typename U, typename A>
    friend class List;
    T val;
    Link* succ;
    Link* prev;
};

template <typename T, typename Allocator>
class List{
    //...
};

//嵌入类不是不是模版的实现细节，只是一个关联类型
template <typename T, typename A>
class BadList2 {
public:
    class Iterator{
        Link<T>* current_position;
    public:
        //...常用操作
    };

    //template <typename T, typename A>
    Iterator begin(){return Iterator{};}
    //template <typename T, typename A>
    Iterator end(){return Iterator{};}
    //...
};
//由于Iterator是一个成员，它形式上依赖于BadList2的所有参数，T和A；因此使用函数应该这样写：
template <typename A>
void fct(typename BadList2<int, A>::Iterator b, typename BadList2<int, A>::Iterator e){
    //...
    cout << "in template fct" << endl;
}

//辅助类
class MyAllocator{};
class YouAllocator{};

//user 的用法迫使fct必需声明成模版，参数化Allocator，但是这导致严重的代码膨胀
void user(BadList2<int, MyAllocator>& lm, BadList2<int, YouAllocator>& ly){
    cout << "int user " << endl;
    fct<MyAllocator>(lm.begin(), lm.end());
    fct<YouAllocator>(ly.begin(), ly.end());
    cout << DeMangle(typeid(lm.begin()).name()) << endl;
};

// 考虑将Iterator移出类模版
template <typename T>
struct Iterator{
    Link<T>* current_position;
};

//第一个模版参数相同的List2的迭代器就可以相互代替使用了
template <typename T, typename A>
class List2{
public:
    Iterator<T> begin(){return Iterator<T>{};}
    Iterator<T> end(){return Iterator<T>{};}
    //...
};

template <typename T>
void fct2(Iterator<T> b, Iterator<T> e){
    //...
    cout << "int template fct2" << endl;
}

void user2(List2<int, MyAllocator>& lm, List2<int, YouAllocator>& ly){
    cout << "int user2 " << endl;
    fct2(lm.begin(), lm.end());
    fct2(lm.begin(), lm.end());
}

//经验：在模版中尽量避免嵌入类型，除非它们真的依赖所有模版参数，这是"在代码中避免不必要的依赖关系" 原则的一个特例

//7. 友元
//模版类中的友元函数
template <typename T> class Matrix;
template <typename T> class Vector;

//友元函数模版需要先声明，才能在模版类中指定为operator* <>()
template <typename T>
Vector<T> operator* (const Matrix<T>& m , const Vector<T>& v);

template <typename T>
class Vector{
    T v[4];
public:
    Vector(){}
    Vector(std::initializer_list<T> l){
        auto i = 0;
        for(auto x:l){
            v[i++] = x;
            cout << "Vector Constructor "<< x << endl;
        }
    }
    //友元函数名后面的<>是必需的，它清楚的指出友元函数是一个函数模版，如果没有，会被假定是非模版函数
    friend Vector operator* <> (const Matrix<T>&, const Vector&);
    template <typename U>
    friend class Matrix;
    //...
};

template <typename T>
class Matrix{
    Vector<T> v[4];
public:
    Matrix(std::initializer_list<T> l[4]){
        for(auto i = 0, j=0; i<4; i++, j=0){
            auto x = l[i];
            for(auto y : x) {
                v[i].v[j++] = y;
                cout << "Matrix Constructor " << y << endl;
            }
        }
    }
    friend Vector<T> operator* <> (const Matrix&, const Vector<T>&);
    //...
};

//有了上述定义，乘法运算符可以直接访问Matrix和Vector中的数据
template <typename T>
Vector<T> operator* (const Matrix<T>& m , const Vector<T>& v){
    Vector<T> r;
    //...
    for(int i = 0; i<4; i++){
        r.v[i] = 0;
        for(int j = 0; j<4; j++){
            r.v[i] += m.v[i].v[j]*v.v[j];
        }
        cout << " i " << i << ", r.v[i] = " << r.v[i] << endl;
    }
    return r;
}

//模版类中指定其他类为friend
class C;
using C2 = C;
template <typename T>
class My_class {
    friend C;   // 正确
    friend C2; // 正确， 别名
    //friend C3; // 错误，在作用域中不存在C3
    friend class C4; //正确，引入一个新的类C4

    // 友元依赖于模版实参
    friend T;   // 参数T是友元
    friend Vector<T>; //模版 和参数T一起构成友元
    //friend class T; // 错误， 'class' 是多余的
};

//友元关系既不能继承也不能传递；
//不能直接将模版定义为一个类的友元，但可以将一个友元声明改为一个模版：
template <typename T, typename A>
class FriendList;

template <typename T>
class FriendLink{
    template <typename U, typename A>
    friend class FriendList;
};


}// namespace member
}//namespace template
#endif //CPPTEMPLATE_MEMBER_H
