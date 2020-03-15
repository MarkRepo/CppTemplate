//
// Created by markfqwu on 2020-03-14.
//

#ifndef CPPTEMPLATE_TECHNIQUES2_H
#define CPPTEMPLATE_TECHNIQUES2_H

#include <string>
#include <iostream>

using namespace std;

namespace modern{
namespace tech2{

//1。 编译时断言，定制错误消息
template <bool>
class CompileTimeChecker{
public:
    CompileTimeChecker(...){
        cout << "in CompileTimeChecker constructor" << endl;
    }
};

template <> class CompileTimeChecker<false>{};

//CompileTimeChecker 和ERROR类型对象没有被真正计算，只在编译期被检查
#define STATIC_CHECK(expr, msg) \
{\
    class ERROR_##msg {public: ERROR_##msg(){cout<<"In ERROR obj Contrutor"<<endl;}};\
    (void)sizeof(CompileTimeChecker<(expr)>(ERROR_##msg{}));\
}

template <typename To, typename From>
To safe_reinterpret_cast(From from){
    STATIC_CHECK(sizeof(From)<=sizeof(To), Destination_Type_Too_Narrow);
    return reinterpret_cast<To>(from);
}

//2。 局部类
//local class不能定义static成员变量，也不能访问non-static 局部变量。定义于template函数内的local class可以使用函数的template参数

class Interface{
public:
    virtual void Fun() = 0;
};

template <typename T, typename P>
Interface* MakeAdapter(const T& obj, const P& arg){
    static int b = 6; //不能访问non-static局部变量
    class Local : public Interface{
    public:
        Local(const T& obj, const P& arg): obj_{obj}, arg_{arg}{}
        virtual void Fun(){
            obj_.Call(arg_);
            cout << b<<endl;
        }

    private:
        T obj_;
        P arg_;

        //static int s;//not allowed， can't define static data member
    };
    return new Local{obj, arg};
};

template <typename T>
class CallObjType{
public:
    void Call(T& arg){cout << "CallObjType exec Call"<<endl;}
};

//3. 使用Int2Type实现编译期分派
template<int v>
class Int2Type{
public:
    enum {value = v};
};

template <typename T, bool isPolymorphic>
class NiftyContainer{
public:
    void DoSomething(T* pObj){
        DoSomething(pObj, Int2Type<isPolymorphic>());
    }

private:
    void DoSomething(T* pObj, Int2Type<true>){
        cout << "In DoSomething Int2Type<true>" << endl;
    }

    void DoSomething(T* pObj, Int2Type<false>){
        cout << "In DoSomething Int2Type<false>" << endl;
    }
};

//4. Type2Type，为了获得类型信息，同时避免创建对象的手法
class Widget{
public:
    Widget(int a, int b){}
};

template <typename T>
class Type2Type{
public:
    using type = T;
};

template <typename T, typename U>
T* Create(const U& arg, Type2Type<T>){
    cout <<"Enter general Create" <<endl;
    return new T{arg};
}

template <typename U>
Widget* Create(const U& arg, Type2Type<Widget>){
    cout << "Enter Widget Create" << string(__PRETTY_FUNCTION__) <<endl;
    return new Widget(arg, -1);
};

//5。 型别选择，参考MetaProgramming
//6. 编译期间检测类型可转换性，继承性
//sizeof可以用在任何表达式上，不论后者有多复杂。sizeof会直接传回大小，不需要拖到执行期才评估。这意味者sizeof可以感知重载，模版具现，转换规则，
//或者任何可以发生在C++表达式身上的机制，sizeof背后隐藏了"类型推导"，最终会丢弃表达式并传回大小。
//基本思想是运用sizeof和重载，为了知道哪一个函数被调用，对两个重载函数安排大小不同的返回类型，并以sizeof区分

template<typename T, typename U>
class Conversion{
    using Small = char;
    class Big{ char dummy[2]; };
    //sizeof 不对参数求值，只做类型推导，返回大小，所以这里都可以不提供实现
    static Small Test(U);
    //这里需要一个在自动转换失效的情况下被选中的函数版本，可变参数符合这个规则；由于模版会精确匹配，屏蔽了类型转换，因此这里不适用
    static Big Test(...);
    //为了防止T类型的构造函数被设计为private而设计的"稻草人"函数
    static T MakeT();

public:
    enum {exists = sizeof(Test(MakeT())) == sizeof(Small)}; //表示T可以转化为U
    enum {exists2way = exists && Conversion<U,T>::exists}; //可相互转换
    enum {sameType = false};
};

//通过偏特化实现sameType
template <typename T>
class Conversion<T,T>{
    enum{exists = 1 , exists2way = 1, sameType = 1};
};

//是否具有继承关系
#define SUPERSUBCLASS(T,U) \
    (Conversion<const U*,const T*>::exists && \
    !Conversion<const T*, const void*>::sameType)

//const U* 与const T*做可转换性评估时，只有三种情况const U* 可转换为const T*
//1. T ,U 是同一型别
//2. T是U的 unambigious public base
//3. T是void

//所以更严格判断，可以去除同一型别
//加上const修饰是为了不希望因const而导致转型失败
#define SUPERSUBCLASS_STRICK(T,U)\
    (SUPERSUBCLASS(T, U) && \
    !Conversion<const T, const U>::sameType)

//7。 type_info的一个封装类, 参考loki库，https://github.com/MarkRepo/loki-0.1.7.git
//8。 NullType, EmptyType,参考loki库，https://github.com/MarkRepo/loki-0.1.7.git
//9。 TypeTraits,参考loki库，https://github.com/MarkRepo/loki-0.1.7.git

}//namespace tech
}//namespace modern

#endif //CPPTEMPLATE_TECHNIQUES2_H
