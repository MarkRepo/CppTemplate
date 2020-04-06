//
// Created by markfqwu on 2020-03-15.
//

#ifndef CPPTEMPLATE_TYPELISTS_H
#define CPPTEMPLATE_TYPELISTS_H
#include <iostream>
#include <string>

#include "ModernCpp/techniques2.h"
#include "CommonTools/typeinfodemangle.h"

using namespace std;

using modern::tech2::NullType;
using modern::tech2::EmptyType;
using modern::tech2::Select;
using modern::tech2::Conversion;

using tpl::common::DeMangle;

namespace modern{
namespace TL{

//typelist没有定义状态，一般用于编译期，目的是为了携带类型信息
template<typename T, typename U>
struct TypeList{
    using Head = T;
    using Tail = U;
};

//以NullType标记typelist的结尾
//class NullType;
//struct EmptyType{};

//将typelist的生成线性化
#define TYPELIST_1(T1) TypeList<T1,NullType>
#define TYPELIST_2(T1, T2) TypeList<T1, TYPELIST_1(T2)>
#define TYPELIST_3(T1,T2,T3) TypeList<T1, TYPELIST_2(T2, T3)>
#define TYPELIST_4(T1,T2,T3,T4) TypeList<T1, TYPELIST_3(T2, T3, T4)>

//todo loki支持到了50个, 这里考虑用变长模版参数优化typelist的实现

//typelist基本操作, 编译期计算不兼容迭代，因为没有"可资变化的任何东西"

/*
 * 0 TypeListStr 打印类型串
 * 输入typelist TList
 * 输出 string s
 * 如果 TList是NullType, s 为""
 * 否则
 *      s 为Demangle(typeid(TList::Head).name()) 与 TypeListStr<Tail>::s的和
 *
 */

template <typename TList> struct TypeListStr;

template <> struct TypeListStr<NullType>{
    static string s;
    string str = "NullType";
};

string TypeListStr<NullType>::s = "NullType";

template <typename Head, typename Tail>
struct TypeListStr< TypeList<Head, Tail>>{

    string str = DeMangle(typeid(Head).name()) + "-" + TypeListStr<Tail>().str;
    static string s;
};

//static 成员变量方式会导致NullType不会被递归
template <typename Head, typename Tail>
string TypeListStr<TypeList<Head, Tail>>::s = DeMangle(typeid(Head).name()) + "-" + TypeListStr<Tail>::s;

//1 计算长度

template <typename TList> struct Length;
template <> struct Length<NullType> {
    enum {value = 0};
};

template <typename T, typename U>
struct Length< TypeList<T,U> > {
    enum {value = 1 + Length<U>::value}; //递归实现
};

/*
 * 2 TypeAt算法：
 * 输入：typelist TList， 索引值 i
 * 输出：内部某型别Result
 *
 * 如果TList 非null， 且i为0， 那么Result就是TList的头部
 * 否则
 *      如果TList不为null且i不为0， 那么Result就是"将TypeAt施行于TList尾端及i-1"的结果
 *      否则，越界访问，造成编译错误
 */

template <typename TList, unsigned int index> struct TypeAt;

template <typename Head, typename Tail>
struct TypeAt<TypeList<Head,Tail>, 0>{
    using Result = Head;
};

template <typename Head, typename Tail, unsigned int i>
struct TypeAt<TypeList<Head, Tail>, i>{
    using Result = typename TypeAt<Tail, i-1>::Result; //递归实现
};

/*
 * 3 IndexOf算法
 * 输入：typelist TList， type T
 * 内部编译期常数 value
 *
 * 如果TList为NullType， 令value=-1
 * 否则
 *      如果TList头端是T，令value为0
 *      否则
 *          将IndexOf施行于"Tlist 尾端Tail和T"，并将结果至于一个临时变量tmp
 *          如果tmp为-1， 则value为-1
 *          否则令value为 1+tmp
 */

template <typename TList, typename T> struct IndexOf;

template <typename T>
struct IndexOf<NullType, T>{
    enum {value = -1};
};

template <typename Tail, typename T>
struct IndexOf<TypeList<T, Tail>, T>{
    enum {value = 0};
};

template <typename Head, typename Tail, typename T>
struct IndexOf< TypeList<Head, Tail>, T >{
private:
    enum { tmp = IndexOf<Tail, T>::value }; // 递归实现
public:
    enum { value = tmp == -1 ? -1 : 1+tmp };
};


/*
 * 4 Append 算法：
 * 输入： typelist TList， type or typelist T
 * 输出： 内部某型别Result
 *
 * 如果TList 是 NullType且 T是NullType， 则Result为NullType
 * 否则
 *      如果TList是NullType 且T是个Type而非TypeList， 那么Result为只含一个元素T的typelist
 *      否则
 *          如果TList是NullType，且T是个TypeList，那么Result是T本身
 *          否则，如果TList是non-null， 那么Result将是个typelist，以TList::Head为头端，并以T附加至TList::Tail的结果为其尾端
 */

template <typename TList, typename T> struct Append;

template <> struct Append<NullType, NullType>{
    using Result = NullType;
};

template <typename T>
struct Append<NullType, T>{
    using Result = TYPELIST_1(T);
};

template <typename Head, typename Tail>
struct Append<NullType, TypeList<Head, Tail> >{
    using Result = TypeList<Head, Tail>;
};

template <typename Head, typename Tail, typename T>
struct Append< TypeList<Head, Tail>, T>{
    using Result = TypeList<Head, typename Append<Tail, T>::Result >;  //递归实现
};


/*
 * 5 Erase算法： 只移除第一个出现者
 * 输入： TypeList TList， type T
 * 输出： 内部某型别Result
 *
 * 如果TList是NullType， 那么Result就是NullType
 * 否则
 *      如果T等同于TList::Head， 那么Result就是TList::Tail
 *      否则，Result将是一个Typelist，它以TList::Head为头端，并以"将Erase施行于TList::Tail和T"所得结果为其尾端
 */

template <typename TList, typename T> struct Erase;

template <typename T>
struct Erase<NullType, T>{
    using Result = NullType;
};

template <typename Tail, typename T>
struct Erase< TypeList<T, Tail>, T>{
    using Result = Tail;
};

template <typename Head, typename Tail, typename T>
struct Erase< TypeList<Head, Tail>, T>{
    using Result = TypeList<Head, typename Erase<Tail, T>::Result >;
};

/*
 * 5.1 EraseAll算法：移除所有出现个体
 * 类似Erase，不同的是，发现移除对象后算法并不停下来，而是继续查找下一个符合条件的元素并删除，知道TList的尾端
 */

template <typename TList, typename T> struct EraseAll;

template <typename T>
struct EraseAll<NullType, T>{
    using Result = NullType;
};

template <typename Tail, typename T>
struct EraseAll< TypeList<T, Tail>, T>{
    using Result = typename EraseAll<Tail, T>::Result; // 递归移除所有符合条件的元素
};

template <typename Head, typename Tail, typename T>
struct EraseAll< TypeList<Head, Tail>, T>{
    using Result = TypeList<Head, typename EraseAll<Tail, T>::Result >;
};

/*
 * 6 NoDuplicates 算法，移除重复元素
 * 输入： Typelist TList
 * 输出： 内部某型别Result
 *
 * 如果TList是NullType， 那么Result是NullType
 * 否则
 *      将NoDuplicates施行于TList::Tail 身上，获得一个暂时的typelist L1
 *      将Erase施行于L1 和 TList::Head, 获得L2 （已经经过NoDuplicate处理，所以Erase就够了，不需要EraseAll）
 *      Result是个typelist， 头端为TList::Head, 尾端为L2
 */

template <typename TList> struct NoDuplicates;

template <> struct NoDuplicates<NullType>{
    using Result = NullType;
};

template <typename Head, typename Tail>
struct NoDuplicates< TypeList<Head, Tail>>{
private:
    using L1 = typename NoDuplicates<Tail>::Result ;
    using L2 = typename Erase<L1, Head>::Result ;
public:
    using Result = TypeList<Head, L2>;
};

/*
 * 7 Replace算法： 替换TypeList中某个元素
 * 输入： Typelist TList， type T（被取代者）， type U（取代者）
 * 输出： 内部某型别Result
 *
 * 如果TList是NullType， 令Result为NullType
 * 否则
 *      如果typelist TList的头端是T， 那么Result将是一个Typelist，以U为其头端，TList::Tail为其尾端
 *      否则 Result是一个typelist，以TList::Head 为其头端，并以"Replace施行于TList::Tail，T, U" 为其尾端
 */

template <typename TList, typename T, typename U> struct Replace;

template <typename T, typename U>
struct Replace <NullType, T, U>{
    using Result = NullType;
};

template <typename Tail, typename T, typename U>
struct Replace< TypeList<T, Tail>, T, U>{
    using Result = TypeList<U, Tail>;
};

template <typename Head, typename Tail, typename T, typename U>
struct Replace< TypeList<Head, Tail>, T, U>{
    using Result = TypeList<Head, typename Replace<Tail, T, U>::Result >;
};

/*
 * 7.1 ReplaceAll算法
 */

template <typename TList, typename T, typename U> struct ReplaceAll;

template <typename T, typename U>
struct ReplaceAll <NullType, T, U>{
    using Result = NullType;
};

template <typename Tail, typename T, typename U>
struct ReplaceAll < TypeList<T, Tail>, T, U>{
    using Result = TypeList<U, typename ReplaceAll<Tail, T, U>::Result> ;
};

template <typename Head, typename Tail, typename T, typename U>
struct ReplaceAll< TypeList<Head, Tail>, T, U>{
    using Result = TypeList<Head, typename ReplaceAll<Tail, T, U>::Result >;
};

/*
 * 8.1 MostDerived算法： 传回Typelist中Base的最深层派生型别（如果找不到派生型别，传回Base自己）
 * 输入： Typelist TList，type T
 * 输出： 内部某型别Result
 *
 * 如果TList为NullType， 令Result为T
 * 否则
 *      将MostDerived施行于TList::Tail 和T身上， 获得一个Candidate
 *      如果TList::head 派生自Candidata, 令Result为TList::head
 *      否则， 令Result为Candidate
 *
 */

template <typename TList, typename T> struct MostDerived;

template <typename T>
struct MostDerived<NullType, T>{
    using Result = T;
};

template <typename Head, typename Tail, typename T>
struct MostDerived< TypeList<Head, Tail>, T>{
private:
    using Candidate = typename MostDerived<Tail, T>::Result ;
public:
    using Result = typename Select<SUPERSUBCLASS(Candidate, Head), Head, Candidate>::Result;
};


/*
 * 8 DerivedToFront：最末端派生型别排在最前面：在一个类继承层次typelist中，调整型别顺序，让所有派生型别出现在基础型别之前， 兄弟型别顺序不变。相当于由下而上遍历class继承体系
 * 如 B、C继承A， D继承C， TypeList<A, B, C, D> ---> TypeList<D, B, C, A>
 * 输入：TypeList TList
 * 输出：内部某型别 Result
 *
 * 如果TList是NullType， 令Result为NullType
 * 否则
 *      从TList::Head到TList::Tail, 找出最末端派生型别，存储于暂时变量TheMostDerived
 *      以TList::Head 取代TList::Tail 中的TheMostDerived, 获得L
 *      建立一个typelist， 以TheMostDerived 为其头端，以L为其尾端
 */

template <typename TList> struct DerivedToFront;

template <> struct DerivedToFront<NullType>{
    using Result = NullType;
};

template <typename Head, typename Tail>
struct DerivedToFront< TypeList<Head, Tail>>{
private:
    using TheMostDerived = typename MostDerived<Tail, Head>::Result ;
    using L = typename Replace<Tail, TheMostDerived, Head>::Result;
public:
    using Result = TypeList<TheMostDerived, L>;
};

/*
 * 9 GenScatterHierarchy 产生散乱的继承体系
 */

template <typename TList, template <typename> class Unit>
class GenScatterHierarchy;

//specialization : TypeList to Unit
template <typename T1, typename T2, template<typename >class Unit>
class GenScatterHierarchy<TypeList<T1, T2>, Unit>:
        public GenScatterHierarchy<T1, Unit>,
        public GenScatterHierarchy<T2, Unit> {};

//Pass a atomicType (non-TypeList) to Unit
template<typename AtomicType, template <typename> class Unit>
class GenScatterHierarchy : public Unit<AtomicType>{};

//DoNothing for NullType
template <template <typename > class Unit>
class GenScatterHierarchy<NullType, Unit>{};

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

using TestInfo = GenScatterHierarchy<TYPELIST_4(int, string, double, float), Holder>;

//derived-to-based
template <typename T, typename TList, template <typename> class Unit>
Unit<T>& Field_bad(GenScatterHierarchy<TList, Unit>& obj){
    return obj;
}

template <typename TList, template<typename> class Unit>
Unit<typename TList::Head>& FieldHelper(GenScatterHierarchy<TList, Unit>& obj, tech2::Int2Type<0>){
    GenScatterHierarchy<typename TList::Head, Unit>& leftBase = obj;
    return leftBase;
}

template <int i, typename TList, template<typename >class Unit>
Unit<typename TypeAt<TList, i>::Result>& FieldHelper(GenScatterHierarchy<TList, Unit>& obj, tech2::Int2Type<i>){
    GenScatterHierarchy<typename TList::Tail, Unit>& rightBase = obj;
    return FieldHelper(rightBase, tech2::Int2Type<i-1>());
}

template <int i, typename TList, template<typename >class Unit>
Unit<typename TypeAt<TList,i>::Result>& Field(GenScatterHierarchy<TList, Unit>& obj){
    return FieldHelper(obj, tech2::Int2Type<i>());
}

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
    class T1,
    class T2,
    template<typename, typename> class Unit,
    class Root
>
class GenLinearHierarchy<TypeList<T1, T2>, Unit, Root>
        :public Unit<T1, GenLinearHierarchy<T2,Unit,Root>>{
public:
    using base = Unit<T1, GenLinearHierarchy<T2,Unit,Root>>;
};

//递归停止点
template <typename T, template <class, class> class Unit, class Root>
class GenLinearHierarchy<TypeList<T, NullType>, Unit , Root>
        :public Unit<T, Root>{
public:
    using base = Unit<T, Root>;
};

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

using MyEventHandler = GenLinearHierarchy<TYPELIST_3(Window, Button, ScrollBar), EventHandler>;

}//namespace TL
}//namespace modern

#endif //CPPTEMPLATE_TYPELISTS_H
