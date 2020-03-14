//
// Created by markfqwu on 2020-03-13.
//

#ifndef CPPTEMPLATE_TECHNIQUES_H
#define CPPTEMPLATE_TECHNIQUES_H

#include <string>
#include <iostream>
#include <mutex>

using namespace std;

namespace modern{
namespace tech {

//软件设计是解域空间的一道选择题
//系统架构的一个主要基本原则是以设计实现某些原则，如不能产生两个singleton对象
//1 全功能型接口的失败
//2 多重继承的缺点
//比较模版和多重继承的区别，两者互补：
//多重继承欠缺技术,templates有丰富的技术；多重继承缺乏型别信息，而templates大量存在；Templates无法扩张，多重继承很容易扩张
//只能为template的成员函数写一份缺省版本，但可以写数量无限的base classes。两者组合，可以产生非常具备弹性的设备

//1.1 无法特化结构(数据成员)，只能特化成员函数的验证：
template<typename T>
class Member {
public:
    void echo() { cout << "a_ is " << a_ << ", s_ is " << s_ << endl; }

    Member(int a, string s) : a_{a}, s_{s} {}

private:
    int a_;
    string s_;
};

template<>
class Member<float> {
public:
    void echo() { cout << d_ << " and " << f_ << endl; }

    Member(double d, float f) : d_{d}, f_{f} {}

private:
    double d_;
    float f_;
};

//Member<char> echo成员函数的特化
template <> void Member<char>::echo(){
    cout << "echo for char" << endl;
}



//1.2 成员函数的特化不能依理扩张，可以对单一template参数的class template 特化其成员函数，却无法对多个template 参数的class template特化其成员函数
//已支持
template<typename T, typename U>
class MemberFunc {
public:
    void Func() { cout << "MemberFunc for T U" << endl; }
};

template<typename U>
class MemberFunc<int, U> {
public:
    void Func() { cout << "MemberFunc for int U" << endl; }
};

//1.3. nested name specifier 'MemberFunc<char, U>::' for declaration does not
//refer into a class, class template or class template partial specialization
/*
template <typename U>
void MemberFunc<char, U>::Func(){
    cout << "MemberFunc for char U" << endl;
}*/


//2 policies and policy classes
//2.1 operator new policy class
template <typename T>
struct OpNewCreator{
    //Create()就是policy接口，每个policy class必需定义该方法
    static T* Create(){
        cout << "Enter OpNewCreator Create()" << endl;
        return new T;
    }
};

//2.2 malloc policy class
template <typename T>
struct MallocCreator{
    static T* Create(){
        cout << "Enter MallocCreator Create()" << endl;
        void* buf = malloc(sizeof(T));
        if(!buf) return nullptr;
        return new(buf) T;
    }
};

//2.3 prototype policy class
template <typename T>
struct PrototypeCreator{
    PrototypeCreator(T* pObj = nullptr): pObj_{pObj}{}
    T* Create(){
        cout << "Enter PrototypeCreator Create()" << endl;
        return pObj_?pObj_->clone(): nullptr;
    }
    T* GetObj(){return pObj_;}
    void SetObj(T* pObj){pObj_ = pObj;}

private:
    T* pObj_;
};

//以继承或组合的方式使用policy classes
//如果class采用一个或多个policies，称其为hosts，或host classes，Hosts负责把policy提供的行为组合成一个更复杂的结构和行为
class Widget{
public:
    void echo(){cout << "Widget Echo" << endl;}
};

template<typename CreationPolicy>
class WidgetManager:public CreationPolicy{

};

using MyWidgetMgr = WidgetManager<OpNewCreator<Widget>>;

class Gadget{};

//3. 使用template template参数，使其更合理
//3.1 Created是CreationPolicy的形式模版参数，不能在WidgetManager中使用, 可以省略
//3.2 使用template template 参数，不只是为了方便，同时可借由template产生不同型别的对象,如使用相同策略生成Gadget对象
template <template <typename Created> class CreationPolicy>
class WidgetManager2 : public CreationPolicy<Widget>{
public:
    void DoSomething(){
        Gadget* g = CreationPolicy<Gadget>().Create();
    }
};

//更通用的Mgr, 同时带缺省模版OpNewCreator
template <typename T,  template <typename > class CreationPolicy = OpNewCreator>
class WidgetManager3 : public CreationPolicy<T>{

};
using MyWidgetMgr2 = WidgetManager2<OpNewCreator>; // 只需要提供模版名
using MyWidgetMgr3 = WidgetManager3<Widget, OpNewCreator>;

//4. 思考policies和虚函数的区别

//5. policy classes 的析构函数
//由于host class public继承policy class，使用者可以将host* 转化为policy*， 然后delete该指针，这导致policy class需要定义一个虚析构函数，否则
//会导致内存泄露；但是虚函数会妨碍policy class的静态链接特性，同时会影响执行效率，增加对象内存开销（vptr），所以应该避免虚析构函数；
//一个解法是，public 继承改为protecetd或private继承，但是会失去policy的丰富特性
//轻便而有效率的解法是：policy class定义一个non-virtial protected 析构函数
//由于析构函数是protected，只有派生而得的class才可以销毁这个policy对象。
 template <typename T>
 class OpNewCreator2{
 public:
     static T* Create(){
         return new T;
     }

 protected:
     ~OpNewCreator2(){cout << "~OpNewCreator2()" << endl;}
 };

 using MyWidgetMgr22 = WidgetManager2<OpNewCreator2>;

//6 通过不完全具现化获得选择性机能
//6.1. 如果选择支持prototype的policy，则可以使用SwitchObj
//6.2. 如果选择不支持prototype的policy，并使用SwitchObj，编译报错
//6.3. 如果选择不支持prototype的policy，不使用SwitchObj，合法

template <template <typename> class CreationPolicy>
class WidgetManager4 : public CreationPolicy<Widget>{
public:

    void SwitchObj(Widget* pObj){
        cout << "Enter Switch Obj" << endl;
        delete this->GetObj();
        this->SetObj(pObj);
    }

};

template<typename T1>
class EnforceNotNull;

//7.组合policy class
template <typename T>
struct NoChecking{

    NoChecking(){}
    //为了policies之间的兼容性，定义NoChecking之间的转换，参考9条
    template <typename T1>
    NoChecking(NoChecking<T1>& other){
        cout << "NoChecking inter transfer "<< endl;
    }

    //为了policies之间的兼容性，定义NoChecking转换为EnforNotNull的类型转换操作符,参考9条
    template <typename T1>
    operator EnforceNotNull<T1>(){
        cout << "NoChecking trans to EnforceNotNull operator" << endl;
    }

    static void Check(T*){cout<< "NoChecking"<<endl;}
};

template <typename T>
class EnforceNotNull{
public:
    class EnforceNotNullException : public std::exception{
    public:
        const char* what() const noexcept override {
            return "this is EnforceNotNullException";
        }
    };

    EnforceNotNull(){}
    //为了policies之间的兼容性，定义NoChecking转换为EnforNotNull,参考9条
    //如果没有这个定义，NoChecking中的类型转换操作符会生效
    template <typename T1>
    EnforceNotNull(NoChecking<T1>& other){
        cout << "NoChecking transfer to EnforceNotNull " << endl;
    }

    static void Check(T* ptr){
        cout << "EnforeceNotNull" <<endl;
        if(!ptr)throw EnforceNotNullException();
    }
};

template <typename T>
class SingleThreaded{
public:
    class Lock{
    public:
        Lock(T& obj){
            cout << "Lock Obj" << endl;
            mu_.lock();
        }
        ~Lock(){
            cout << "UnLock Obj" << endl;
            mu_.unlock();
        }
    private:
        std::mutex mu_;
    };
};

template <typename T,
        template <typename >class CheckingPolicy,
        template <typename >class ThreadingModel>
class SmartPtr : public CheckingPolicy<T>, public ThreadingModel<T>{
public:
    SmartPtr(T* pObj){
        pObj_ = pObj;
    }

    T*operator->(){
        cout << "Enter SmartPtr dereference" << endl;
        typename ThreadingModel<T>::Lock guard(*pObj_);
        CheckingPolicy<T>::Check(pObj_);
        return pObj_;
    }

private:
    T* pObj_;
};

using WidgetPtr = SmartPtr<Widget, NoChecking, SingleThreaded>;
using SafeWidgetPtr = SmartPtr<Widget, EnforceNotNull, SingleThreaded>;


//8. 以policy class 定制结构，这里讨论通过一个Structure policy将指针的存储概念抽象化，以应对"非指针形式"的SmartPtr
//缺省存储一个带有Sturcture policy接口的一般指针：
template <typename T>
class DefaultSmartPtrStorage{
public:
    using PointerType = T*;
    using ReferenceType = T&;
protected:
    PointerType GetPointer(){return pointee_;}
    void SetPointer(PointerType pointee){pointee_ = pointee;}

private:
    PointerType pointee_;
};

//使用storage取代对T*的聚合
template <typename T,
        template <typename > class CheckingPolicy,
        template <typename > class ThreadingModel,
        template <typename > class Storage = DefaultSmartPtrStorage
        >
class SSmartPtr : public CheckingPolicy<T>, public ThreadingModel<T>, public Storage<T>{
public:
    SSmartPtr(typename Storage<T>::PointerType ptr){
        this->SetPointer(ptr);
    }
    typename Storage<T>::PointerType operator->(){
        cout << "Enter SSmart dereference" << endl;
        typename ThreadingModel<T>::Lock guard(*(this->GetPointer()));
        CheckingPolicy<T>::Check(this->GetPointer());
        return this->GetPointer();
    }
};

using SWidgetPtr = SSmartPtr<Widget, NoChecking, SingleThreaded>;
using SSafeWidgetPtr = SSmartPtr<Widget, EnforceNotNull, SingleThreaded>;

//9 policies 之间的兼容性
//policies 之间的转换： 以policies来控制SmartPtr的拷贝和初始化：
template<typename T,
        template <typename> class CheckingPolicy
        >
class CSmartPtr : public CheckingPolicy<T>{
public:

    CSmartPtr(){}
    template <typename T1,
            template <typename >class CP1
             >
    CSmartPtr(CSmartPtr<T1, CP1>& other):pObj_{other.pObj_}, CheckingPolicy<T>(other){}

    T* pObj_;

};

class ExtWidget : public Widget{};

using CWidgetPtr = CSmartPtr<Widget, NoChecking>;
using CSafeWidgetPtr = CSmartPtr<Widget, EnforceNotNull>;
using ECWidgetPtr = CSmartPtr<ExtWidget, NoChecking>;
using ECSafeWidgetPtr = CSmartPtr<ExtWidget, EnforceNotNull>;

//9.1。如果用CSmartPtr<ExtWidget, NoChecking>初始化CSmartPtr<Widget, NoChecking>相当于用ExtWidget*初始化Widget*, CSmartPtr<ExtWidget, NoChecking>
//初始化Nochecking，这可以运行

//9.2。如果用CSmartPtr<ExtWidget, NoChecking>初始化CSmartPtr<Widget, EnforceNotNull>,相当于CSmartPtr<ExtWidget, NoChecking>初始化
//EnforceNotNull, 这需要EnforceNotNull 有可接受NoChecking对象的构造函数，或者NoChecking 有将自己转换为EnforceNotNull的转型操作符

//10 将一个class分解为一堆policies
//10.1。 将参与class行为的设计鉴别出来并命名之，任何事情只要能用一种以上的方法解决，都应该被分析出来，并从class中移除出来成为policy
//10.2。过于泛化的host class会产生缺点，它会有过多的 template参数，4～6个以上的template参数会造成合作上的笨拙
//10.3。将class分解为policies，找到正交分解很重要，两个policy之间是独立的称为正交。
//10.4。如果必需使用非正交policies，可以"把policy class当作参数传给其他policy class template function" 来降低相依性，缺点是降低了封装性

} // namespace tech
}//namespace modern

#endif //CPPTEMPLATE_TECHNIQUES_H
