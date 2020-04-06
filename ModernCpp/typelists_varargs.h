//
// Created by markfqwu on 2020-04-01.
//

#ifndef CPPTEMPLATE_TYPELISTS_VARARGS_H
#define CPPTEMPLATE_TYPELISTS_VARARGS_H

#include <iostream>
#include <string>
#include <type_traits>

#include "ModernCpp/techniques2.h"
#include "CommonTools/typeinfodemangle.h"

using namespace std;
using modern::tech2::NullType;
using modern::tech2::EmptyType;
using modern::tech2::Select;
using modern::tech2::Conversion;
using tpl::common::DeMangle;

namespace modern{
namespace varargstl{
//用可变模版参数实现TypeList
//0个类型的TypeList实现,用于终止迭代
template <typename... Tails> struct TypeList{
private:
    static string str;
public:

    //0 toString()
    static string toString(){return str;}
    //1 length
    enum {length = 0};
    //2 IndexOf
    template <typename T1>
    struct IndexOf{
        enum {value = -1};
    };
    //3 Append
    template <typename T>
    struct append{
        using Result = TypeList<Tails..., T>;
    };
    template <typename... Tails1>
    struct append<TypeList<Tails1...>>{
        using Result = TypeList<Tails..., Tails1...>;
    };
    template <typename T>
    using Append = typename append<T>::Result;
    //4 Erase
    template <typename T>
    struct erase{
        using Result = TypeList<>;
    };
    template <typename T>
    using Erase = typename erase<T>::Result;
    //5 EraseAll
    template <typename T>
    struct eraseall{
        using Result = TypeList<>;
    };
    template <typename T>
    using EraseAll = typename eraseall<T>::Result ;
    //6 NoDuplicates
    using NoDuplicates = TypeList<>;
    //7 Replace
    template<typename T, typename U>
    struct replace{
        using Result = TypeList<>;
    };
    template <typename T, typename U>
    using Replace =typename  replace<T,U>::Result ;
    //8 ReplaceAll
    template <typename T, typename U>
    struct replaceall{
        using Result = TypeList<>;
    };
    template <typename T, typename U>
    using ReplaceAll  = typename replace<T,U>::Result;
    //9 MostDerived
    template <typename T>
    struct mostderived{
        using Result = T;
    };
    template <typename T>
    using MostDerived = typename mostderived<T>::Result ;
    using DerivedToFront = TypeList<>;
    //11 GenScatterHierarchy
    template<template <typename> class Unit>
    class GenScatterHierarchy{};
};

template<typename... Tails>
string TypeList<Tails...>::str = "";

//至少有一个类型的TypeList实现
template<typename Head, typename... Tails>
struct TypeList<Head, Tails...>{

    static string str;
    using HeadType = Head;
    using Tail = TypeList<Tails...>;
    //0 toString()
    static string toString(){return str;}
    //1 length
    enum {length = 1 + Tail::length};
    //2 TypeAt
    template <unsigned int i>
    struct typeat{
        using Result = typename Tail::template TypeAt<i-1>;
    };
    template<>
    struct typeat<0>{
        using Result = Head;
    };
    template <unsigned int i>
    using TypeAt = typename typeat<i>::Result;
    //3 IndexOf
    template <typename T>
    struct IndexOf{
    private:
        enum {tmp = Tail::template IndexOf<T>::value};
    public:
        enum {value = tmp == -1 ? -1 : 1+tmp};
    };
    template <>
    struct IndexOf<Head>{
        enum {value = 0};
    };
    //4 Append
    template <typename T>
    struct append{
        using Result = TypeList<Head, Tails..., T>;
    };
    template <typename... Tails1>
    struct append<TypeList<Tails1...>>{
        using Result = TypeList<Head, Tails..., Tails1...>;
    };
    template <typename T>
    using Append = typename append<T>::Result;
    //5 Erase
    template <typename T>
    struct erase{
        using Result = typename TypeList<Head>::template Append<typename Tail::template Erase<T>>;
    };
    template <>
    struct erase<Head>{
        using Result = Tail;
    };
    template <typename T>
    using Erase = typename erase<T>::Result ;
    //6 EraseAll
    template <typename T>
    struct eraseall{
        using Result = typename TypeList<Head>::template Append<typename Tail::template EraseAll<T>>;
    };
    template <>
    struct eraseall<Head>{
        using Result = typename Tail::template EraseAll<Head>;
    };
    template <typename T>
    using EraseAll = typename eraseall<T>::Result;
    //7 NoDuplicates
    struct nodup{
    private:
        using L1 = typename Tail::NoDuplicates;
        using L2 = typename L1::template Erase<Head>;
    public:
        using Result = typename TypeList<Head>::template Append<L2>;
    };
    using NoDuplicates =typename nodup::Result ;
    //8 Replace
    template <typename T, typename U>
    struct replace{
        using Result = typename TypeList<Head>::template Append<typename Tail::template Replace<T,U>>;
    };
    template <typename U>
    struct replace<Head, U>{
        using Result = TypeList<U, Tails...>;
    };
    template<typename T, typename U>
    using Replace = typename replace<T,U>::Result;
    //9 ReplaceAll
    template <typename T, typename U>
    struct replaceall{
        using Result = typename TypeList<Head>::template Append<typename Tail::template ReplaceAll<T,U>>;
    };
    template <typename U>
    struct replaceall<Head, U>{
        using Result = typename TypeList<U>::template Append<typename Tail::template ReplaceAll<Head, U>>;
    };
    template <typename T, typename U>
    using ReplaceAll = typename replaceall<T,U>::Result ;
    //10 DerivedToFront
    template <typename T>
    struct mostderived{
    private:
        using Candidate = typename Tail::template MostDerived<T>;
    public:
        using Result = typename Select<std::is_base_of<Candidate, Head>::value, Head, Candidate>::Result;
    };
    template <typename T>
    using MostDerived = typename mostderived<T>::Result;

    struct derivedtofront{
    private:
        using TheMostDerived = typename Tail::template MostDerived<Head> ;
        using L = typename Tail::template Replace<TheMostDerived, Head>;
    public:
        using Result = typename TypeList<TheMostDerived>::template Append<L>;
    };
    using DerivedToFront = typename derivedtofront::Result;
};

template<typename Head, typename ... Tails>
string TypeList<Head, Tails...>::str = Tail::length > 0 ? DeMangle(typeid(Head).name()) + " " + Tail::toString() : DeMangle(typeid(Head).name());

template <typename T>
class Holder{
public:
    T value_;
};
//for test
template <>
class Holder<int>{
public:
    int value_ = 1;
};
template <>
class Holder<string>{
public:
    string value_ = "abc";
};

/*
 * 9 GenScatterHierarchy 产生散乱的继承体系
 */
template <typename TList, template <typename> class Unit>
class GenScatterHierarchy;
//specialization : TypeList to Unit
template <typename Head, typename... Tails, template<typename >class Unit>
class GenScatterHierarchy<TypeList<Head, Tails...>, Unit>:
        public GenScatterHierarchy<Head, Unit>,
        public GenScatterHierarchy<TypeList<Tails...>, Unit> {};

//Pass a atomicType (non-TypeList) to Unit
template<typename AtomicType, template <typename> class Unit>
class GenScatterHierarchy : public Unit<AtomicType>{};
//DoNothing for TypeList<>
template <template <typename > class Unit>
class GenScatterHierarchy<TypeList<>, Unit>{};

//derived-to-based
template <typename T, typename TList, template <typename> class Unit>
Unit<T>& Field_bad(GenScatterHierarchy<TList, Unit>& obj){ return obj; }

template <typename TList, template<typename> class Unit>
Unit<typename TList::HeadType>& FieldHelper(GenScatterHierarchy<TList, Unit>& obj, tech2::Int2Type<0>){
    GenScatterHierarchy<typename TList::HeadType, Unit>& leftBase = obj;
    return leftBase;
}

template <int i, typename TList, template<typename >class Unit>
Unit<typename TList::template TypeAt<i>>& FieldHelper(GenScatterHierarchy<TList, Unit>& obj, tech2::Int2Type<i>){
    GenScatterHierarchy<typename TList::Tail, Unit>& rightBase = obj;
    return FieldHelper(rightBase, tech2::Int2Type<i-1>());
}

template <int i, typename TList, template<typename >class Unit>
Unit<typename TList::template TypeAt<i>>& Field(GenScatterHierarchy<TList, Unit>& obj){
    return FieldHelper(obj, tech2::Int2Type<i>());
}

using TestInfo = GenScatterHierarchy<TypeList<int, string, double, float>, Holder>;

/*
 * 10 GenLinearHierarchy 产生线性继承体系
 */

//（1）GenLinearHierarchy声明
template
<
    typename TList,
    template <typename AtomicType, typename base>class Unit,
    class Root = EmptyType
>
class GenLinearHierarchy;

//（2）TypeList 特化
template
<
    class Head,
    class... Tails,
    template<typename, typename> class Unit,
    class Root
>
class GenLinearHierarchy<TypeList<Head, Tails...>, Unit, Root> : public Unit<Head, GenLinearHierarchy<TypeList<Tails...>,Unit,Root>>{
public:
    using base = Unit<Head, GenLinearHierarchy<TypeList<Tails...>,Unit,Root>>;
};

//递归停止点
template <template <class, class> class Unit, class Root>
class GenLinearHierarchy<TypeList<>, Unit , Root>{};

//利用(2)中的public Unit和 此处的public Base实现线性继承的递归
template <typename T, typename Base>
class EventHandler: public Base{
public:
    using base = Base;
    virtual void OnEvent(T& obj, int eventid){
        cout << "eventid "<< eventid <<" type "<< DeMangle(typeid(T).name()) << endl;
    }
};

class Window{};
class Button{};
class ScrollBar{};
using MyEventHandler = GenLinearHierarchy<TypeList<Window, Button, ScrollBar>, EventHandler>;

}//namespace varargstl
}//namespace modern

#endif //CPPTEMPLATE_TYPELIST_VARARG_H
