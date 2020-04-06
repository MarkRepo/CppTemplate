//
// Created by markfqwu on 2020-03-15.
//

#include "gtest/gtest.h"
#include "ModernCpp/typelists.h"

using namespace modern::TL;

TEST(TypelistsTest, TypeListStr){
    cout << TypeListStr<TYPELIST_4(int,char,float,double)>::s << endl;
    cout << TypeListStr<TYPELIST_1(int)>::s<<endl;
    cout << TypeListStr<TYPELIST_4(int,char,float,double)>().str << endl;
    cout << TypeListStr<TYPELIST_1(int)>().str <<endl;
}

TEST(TypelistsTest, Length){
    ASSERT_EQ((Length<TYPELIST_2(int, double)>::value), 2) << "Length<typelist> error";
    ASSERT_EQ((Length<TYPELIST_3(int,int, double)>::value), 3)<< "Length<typelist> error";
}

TEST(TypelistTest, TypeAt){
    ASSERT_EQ( (typeid(TypeAt<TYPELIST_4(int, double, float, char), 2>::Result).name()), string("f")) << "TypeAt<typelist, index> not work";
}

TEST(TypelistTest, IndexOf){
    ASSERT_EQ( (IndexOf<TYPELIST_4(int,char,double,float), char>::value), 1) << "IndexOf<typelist, index> not work";
    ASSERT_EQ( (IndexOf<TYPELIST_4(int,char,double,float), double>::value), 2) << "IndexOf<typelist, index> not work";
}

TEST(TypelistTest, Append){
    ASSERT_EQ( (TypeListStr< Append<TYPELIST_2(int,char), TYPELIST_2(double,float)>::Result>().str), string("int-char-double-float-NullType"));
    ASSERT_EQ( (TypeListStr< Append<TYPELIST_2(int,char), float>::Result>().str), string("int-char-float-NullType"));
}

TEST(TypelistTest, Erase){
    ASSERT_EQ((TypeListStr< Erase<TYPELIST_4(int,char,double,float), double>::Result>().str), string("int-char-float-NullType"));
}

TEST(TypelistTest, EraseAll){
    ASSERT_EQ((TypeListStr< EraseAll<TYPELIST_4(int,char,int,double), int>::Result>().str), string("char-double-NullType"));
}

TEST(TypelistTest, NoDuplicates){
    ASSERT_EQ((TypeListStr< NoDuplicates<TYPELIST_4(char, int ,char, char)>::Result>().str), string("char-int-NullType"));
}

TEST(TypelistTest, Replace){
    ASSERT_EQ( (TypeListStr< Replace<TYPELIST_4(int, float, char, int), int, double>::Result >().str), string("double-float-char-int-NullType"));
}

TEST(TypelistTest, ReplaceAll){
    ASSERT_EQ( (TypeListStr< ReplaceAll<TYPELIST_4(int, float, char, int), int, double>::Result >().str), string("double-float-char-double-NullType"));
}

class A{};
class B:public A{};
class C:public A{};
class D:public C{};

TEST(TypelistTest, DerivedToFront){
    ASSERT_EQ( (TypeListStr<DerivedToFront< TYPELIST_4(A,B, C, D)>::Result>().str), string("D-B-C-A-NullType"));
    ASSERT_EQ( (TypeListStr<DerivedToFront< TYPELIST_4(int,float,double,char)>::Result>().str), string("int-float-double-char-NullType"));
}

TEST(TypelistTest, GenScatterHierarchy){
    TestInfo obj;

    string name = (static_cast<Holder<string>&>(obj)).value_;
    ASSERT_EQ(name, string("abc"));
    ASSERT_TRUE(SUPERSUBCLASS(Holder<string>, TestInfo));
    auto& value = Field_bad<string>(obj);
    ASSERT_EQ(value.value_, string("abc"));

    ASSERT_EQ(Field<0>(obj).value_, 1);
    ASSERT_EQ(Field<1>(obj).value_, string("abc"));
}

TEST(TypelistTest, GenLinearHierarchy){
    MyEventHandler obj;
    Window window;
    Button button;
    obj.OnEvent(window, 1);
    auto btObj = static_cast<EventHandler<Button, typename MyEventHandler::base::base::base::base>&>(obj);
    btObj.OnEvent(button, 2);
}