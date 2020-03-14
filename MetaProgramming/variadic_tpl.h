//
// Created by markfqwu on 2020-02-06.
//

#ifndef CPPTEMPLATE_VARIADIC_TPL_H
#define CPPTEMPLATE_VARIADIC_TPL_H

#include "CommonTools/typeinfodemangle.h"
#include "MetaProgramming/meta.h"

#include <iostream>
#include <type_traits>
#include <fstream>

using std::cout;
using std::endl;
using tpl::common::DeMangle;
using tpl::meta::Select;

using std::ostream;
using std::runtime_error;

namespace tpl{
namespace variadic{

class Nil{};

template<bool B , typename T = void>
using Enable_if = typename std::enable_if<B, T>::type;

//4元组tuple的实现
template <typename T1=Nil, typename T2=Nil, typename T3=Nil, typename T4=Nil>
class Tuple : private Tuple<T2, T3, T4>{//布局，{T2, T3, T4} 在T1前
public:
    T1 x;
    using Base = Tuple<T2, T3, T4>;
    Base* base(){return static_cast<Base*>(this);}
    const Base* base()const {return static_cast<const Base*>(this);}
    Tuple(const T1& t1,const T2& t2,const T3& t3,const T4& t4):Base{t2,t3,t4}, x{t1}{}
};

template <>
class Tuple<>{ //0 元组
public:
    Tuple(){}
};

template <typename T1>
class Tuple<T1>: private Tuple<>{ //1 元组
public:
    T1 x;
    using Base = Tuple<>;
    Base* base(){return static_cast<Base*>(this);}
    const Base* base()const {return static_cast<const Base*>(this);}
    Tuple(const T1& t1):Base{}, x{t1}{}
};

template <typename T1, typename T2>
class Tuple<T1, T2> : private Tuple<T2>{ // 2 元组
public:
    T1 x;
    using Base = Tuple<T2>;
    Base* base(){return static_cast<Base*>(this);}
    const Base* base()const {return static_cast<const Base*>(this);}
    Tuple(const T1& t1,const T2& t2):Base{t2}, x{t1}{}
};

template <typename T1, typename T2, typename T3>
class Tuple<T1, T2, T3>: private Tuple<T2, T3> { // 3 元组
public:
    T1 x;
    using Base = Tuple<T2, T3>;
    Base* base(){return static_cast<Base*>(this);}
    const Base* base()const {return static_cast<const Base*>(this);}
    Tuple(const T1& t1,const T2&t2,const T3& t3): Base(t2, t3), x{t1}{}
};

//print_elements
template <typename T1, typename T2, typename T3, typename T4>
void print_elements(ostream& os, const Tuple<T1, T2, T3, T4>& t){
    cout << t.x << ",";
    print_elements(os, *t.base());
}

template <typename T1, typename T2, typename T3>
void print_elements(ostream& os, const Tuple<T1, T2, T3>& t){
    cout << t.x << ",";
    print_elements(os, *t.base());
}

template <typename T1, typename T2>
void print_elements(ostream& os, const Tuple<T1, T2>& t){
    cout << t.x << ",";
    print_elements(os, *t.base());
}

template <typename T1>
void print_elements(ostream& os, const Tuple<T1>& t){
    cout << t.x;
    print_elements(os, *t.base());
}

template <>
void print_elements(ostream& os, const Tuple<>& t){
    cout << "";
}

template <typename T1, typename T2, typename T3, typename T4>
ostream& operator<<(ostream& os, const Tuple<T1, T2, T3, T4>& t){
    os << "{";
    print_elements(os, t);
    os << "}";
    return os;
}

template <typename Ret, int N>
struct getNth{
    template<typename T>
    static Ret& get(T& t){
        return getNth<Ret, N-1>::get(*t.base());
    }

    template<typename T>
    static const Ret& get(const T& t){
        return getNth<Ret, N-1>::get(*t.base());
    }
};

template <typename Ret>
struct getNth<Ret, 0>{
    template <typename T>
    static Ret& get(T& t){
        return t.x;
    }

    template <typename T>
    static const Ret& get(const T& t){
        return t.x;
    }
};

template <int N, typename T1, typename T2, typename T3, typename T4>
Select<N, T1, T2, T3, T4>& get(Tuple<T1, T2, T3, T4>& t){
    return getNth<Select<N,T1, T2, T3, T4>,N>::get(t);
}

template <int N, typename T1, typename T2, typename T3, typename T4>
const Select<N, T1, T2, T3, T4>& get(const Tuple<T1, T2, T3, T4>& t){
    return getNth<Select<N,T1, T2, T3, T4>,N>::get(t);
}

template <typename T1, typename T2, typename T3, typename T4>
Tuple<T1, T2, T3, T4> make_tuple(const T1& t1, const T2& t2, const T3& t3, const T4& t4){
    return Tuple<T1, T2, T3, T4>(t1, t2, t3, t4);
}

void printf(const char* s){
    if(s==nullptr) return;
    while (*s){
        if(*s == '%' && *++s != '%' ){
            throw runtime_error("invalid format: missing arguments");
        }
        cout << *s++;
    }
}

template <typename T, typename... Args>
void printf(const char* s, T value, Args... args){
    while(s && *s){
        if(*s == '%' && *++s != '%'){
            cout << value;
            return printf(++s, args...);
        }
        cout << *s++;
    }
    throw runtime_error("extra argument provided to printf");
}

//技术细节
//typename...指明Types是一个模版参数包，Types... 指明args是一个函数参数包， args中每个函数实参的类型是Types中对应的模版实参
//...是一个独立的此法符号，因此可以在他的前后放置空格。表示"0个或多个"的含义
template <typename... Types>
void exp(Types... args){
    cout << "in exp " << endl;
}

//如果你有一个参数包，可以在其后放置一个...将其扩展为它所包含的元素序列
template <typename T, typename... Args>
void exp_call(T& t, Args... args){
    cout <<"in exp_call" << endl;
    exp(args...);
}

//扩展为元素序列不限于函数调用
template <typename ... Bases>
class Exp : public Bases...{
public:
    Exp(const Bases&... b):Bases(b)...{
        cout << "Exp constructor" << endl;
    }
};
//在大多数需要元素列表的地方，都可以用省略号表示零个或多个的含义， 例如：
//1 模版实参列表中
//2 函数实参列表中
//3 初始化起列表中
//4 基类说明符列表中
//5 基类或者成员初始化器列表中
//6 sizeof... 表达式中， sizeof... 用来获得一个参数包中的元素数目

//转发, 模版参数F使得call可接受任何可调用对象，call
template <typename F, typename ... T>
void call(F&& f, T&&... t){
    f(std::forward<T>(t)...); //从t中转发0或多个实参
}

template <typename T>
using Add_reference = typename std::remove_reference<T>::type&;

template <typename T>
using Add_const_reference_test = typename std::remove_reference<T>::type const&;

template <typename T>
using Add_const_reference = typename std::add_const<typename std::remove_reference<T>::type>::type&;

//实现标准库tuple

//tuple<> 0元组特例化的实现，用于终止迭代
template <typename... T>
class tuple{
public:
    constexpr tuple(){}
};

//至少有一个元素的元组的实现
template <typename Head,typename... Tail>
class tuple<Head, Tail...> : private tuple<Tail...>{
    using inherited = tuple<Tail...>;
public:
    constexpr tuple(){}
    tuple(Head v, Tail... vtail):inherited(vtail...), m_head(v){
    //下面这种方式导致 std_make_tuple(s, v, 1.2)编译错误:
    //error: binding value of type 'const vector<...>' to reference to type 'vector<...>' drops 'const' qualifier
    //应该是类型推导有问题
    //tuple(Add_const_reference<Head> v, Add_const_reference<Tail>... vtail):inherited(vtail...), m_head(v){
        //cout << "v type is " << DeMangle(typeid(Add_const_reference<Head>).name()) << endl;
    }

    template <typename... T>
    tuple(const tuple<T...>& other):m_head{other.head()}, inherited{other.tail()}{}

    template <typename... T>
    tuple& operator=(const tuple<T...>& other){
        m_head = other.head();
        tail() = other.tail();
        return *this;
    }

protected:
    Head m_head;
public:
    Add_reference<Head> head(){return m_head;}
    Add_const_reference<Head> head()const{return m_head;}

    inherited& tail(){return *this;}
    const inherited& tail() const {return *this;}
};

template <typename ... Types>
tuple<Types...> std_make_tuple(Types&&... t){
    //return tuple<Types...>{std::forward<Types>(t)...};
    return tuple<Types...>(t...);
}

template <typename Ret, unsigned N>
struct std_getNth{
    template <typename T>
    static Ret& get(T& t){
        return std_getNth<Ret, N-1>::get(t.tail());
    }

    template <typename T>
    static const Ret& get(const T& t){
        return std_getNth<Ret, N-1>::get(t.tail());
    }
};

template <typename Ret>
struct std_getNth<Ret, 0>{
    template <typename T>
    static Ret& get(T& t){
        return t.head();
    }

    template<typename T>
    static const Ret& get(const T& t){
        return t.head();
    }
};

template <size_t N, typename... T>
Select<N, T...>& std_get(tuple<T...>& t){
    return std_getNth<Select<N, T...>, N>::get(t);
}

template <size_t N, typename... T>
const Select<N, T...>& std_get(const tuple<T...>& t){
    return std_getNth<Select<N, T...>, N>::get(t);
}

//第一种方式, 使用结构体屏蔽tuple的类型， 由print推导
template<size_t N> // print element N and following elements
struct print_tuple_t {
    template<typename... T>
    static Enable_if<(N<sizeof...(T))>
    print(ostream& os, const tuple<T...>& t){
        if(N==0){
            os << std_get<0>(t);
        }else{
            os<<", "<< std_get<N>(t);
        }
        print_tuple_t<N+1>::print(os,t); // print the rest of the elements
    }

    template<typename... T>
    static Enable_if<!(N<sizeof...(T))>
    print(ostream&, const tuple<T...>&) {}
};

//第二种方式，直接使用函数推导
template <size_t N, typename... T>
Enable_if<(N<sizeof...(T))> print_tuple(ostream& os, const tuple<T...>& t){
    if(N==0){
        os << std_get<0>(t);
    }else{
        os << "," << std_get<N>(t);
    }
    //os << "," << std_get<N>(t);
    print_tuple<N+1, T...>(os, t);
};

template <size_t N, typename... T>
Enable_if<!(N<sizeof...(T))> print_tuple(ostream& os, const tuple<T...>& t){}

std::ostream& operator << (ostream& os, const tuple<>&) { return os << "{}"; }

template<typename T0, typename ...T>
std::ostream& operator<<(ostream& os, const tuple<T0, T...>& t) {
    os << '{';// << std_get<0>(t);
    print_tuple<0>(os,t);
    os << '}';

    os << '{';
    print_tuple_t<0>::print(os, t);
    os << '}';

    return os;
}

template <typename T>
class const_test{
public:
    //const_test(Add_const_reference<T> s) : m_s(s){
    const_test(const T& s) : m_s(s){
    cout << "const test, s type " << DeMangle(typeid(s).name()) << endl;
    }

private:
    T m_s;
};

template <typename T>
const_test<T> make_const_test(T&& t){
    return const_test<T>{t};
}


class MFB{
public:
    void echo(std::string name, int age, int age2){
        cout << "name is " << name <<", age is "<< age << "age2 is" <<age2<< endl;
    }

    void echo(std::string name , double core){
        cout << "name is " << name << ", core is " << core << endl;
    }
};

class MF:public MFB{
public:
    MF(double core):core_{core}{}
    double core_;
};

template <typename T, typename TB, typename... Args , typename... Args2>
void MF_test(T* obj, void(TB::*method)(Args2...), Args... args){
    int age2 = 500;
    (obj->*method)(std::forward<Args>(args)..., age2);
}

template <typename... Args>
void MF_test2(Args... args, int a){
cout << "MF_test2 " << a <<endl;
}

void fstream_test(char* filename){
    string cmd = string("echo abcd > ") + filename;
    std::system(cmd.data());
    std::ifstream f(filename);
    int port = -1;
    f >> port;
    cout << "read port is " << port << endl;
}

}// namespace variadic
}// namespace tpl

#endif //CPPTEMPLATE_VARIADIC_TPL_H
