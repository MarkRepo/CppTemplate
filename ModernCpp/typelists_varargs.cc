//
// Created by markfqwu on 2020-04-01.
//

#include "gtest/gtest.h"
#include "ModernCpp/typelists_varargs.h"

using namespace modern::varargstl;

TEST(TypelistsTest, str){
    ASSERT_EQ((TypeList<int, char, float, double>::toString()), string("int char float double"));
    cout <<(TypeList<char, float, double,int>::toString())<< endl;
    cout << TypeList<int,int,int>::toString() << endl;
    cout << TypeList<int,double,int>::toString()<<endl;
    ASSERT_EQ((TypeList<>::toString()), string());
}

TEST(TypelistsTest, Length){
    ASSERT_EQ((TypeList<int,char,float, double ,float>::length), 5);
    ASSERT_EQ((TypeList<int, char>::length), 2) << "length error";
    ASSERT_EQ((TypeList<int, char, double>::length), 3)<< "length error";
    ASSERT_EQ((TypeList<>::length), 0);
}

TEST(TypelistTest, TypeAt){
    ASSERT_EQ(DeMangle(typeid(TypeList<int, double,float, char>::TypeAt<2>).name()), string("float"));
    ASSERT_EQ(DeMangle(typeid(TypeList<int, double,float, char>::TypeAt<0>).name()), string("int"));
}

TEST(TypelistTest, IndexOf){
    ASSERT_EQ((TypeList<int,double,float,char>::IndexOf<double>::value), 1);
    ASSERT_EQ((TypeList<int,double,float>::IndexOf<char>::value), -1);
}

class A{};

TEST(TypelistTest, Append){
    using dest_type = TypeList<int, double>::Append<TypeList<int, float,char>>;
    ASSERT_EQ(DeMangle(typeid(dest_type).name()), string("modern::varargstl::TypeList<int, double, int, float, char>"));

    using append_result1 = TypeList<int, double>::Append<double>;
    using append_result2 = TypeList<>::Append<int>;
    using append_result3 = TypeList<>::Append<TypeList<int,double,int>>;
    using append_result4 = TypeList<char,float>::Append<TypeList<>>;

    ASSERT_EQ(DeMangle(typeid(append_result1).name()), string("modern::varargstl::TypeList<int, double, double>"));
    ASSERT_EQ(DeMangle(typeid(append_result2).name()), string("modern::varargstl::TypeList<int>"));
    ASSERT_EQ(DeMangle(typeid(append_result3).name()), string("modern::varargstl::TypeList<int, double, int>"));
    ASSERT_EQ(DeMangle(typeid(append_result4).name()), string("modern::varargstl::TypeList<char, float>"));
}

TEST(TypelistTest, Erase){
    using erase_result = TypeList<char,float,double>::Erase<float>;
    ASSERT_EQ(DeMangle(typeid(erase_result).name()), string("modern::varargstl::TypeList<char, double>"));

    using erase_result2 = TypeList<char,float,double>::Erase<int>;
    ASSERT_EQ(DeMangle(typeid(erase_result2).name()), string("modern::varargstl::TypeList<char, float, double>"));
}

TEST(TypelistTest, EraseAll){
    using eraseall_result = TypeList<char, float, double, int, char, double>::EraseAll<char>;
    ASSERT_EQ(eraseall_result::toString(), string("float double int double"));

    using eraseall_result2 = TypeList<int, char, float>::EraseAll<double>;
    ASSERT_EQ(eraseall_result2::toString(), string("int char float"));
}

TEST(TypelistTest, NoDuplicates){
    using nodup_result = TypeList<char, float, double, int, char, double>::NoDuplicates;
    ASSERT_EQ(nodup_result::toString(), string("char float double int"));
}

TEST(TypelistTest, Replace){
    using replace_result = TypeList<char, float, double, int, char, double>::Replace<char, double>;
    ASSERT_EQ(replace_result::toString(), string("double float double int char double"));
}

TEST(TypelistTest, ReplaceAll){
    using replaceall_result = TypeList<char, float, double, int, char, double>::ReplaceAll<char, double>;
    ASSERT_EQ(replaceall_result::toString(), string("double float double int double double"));
}

class B:public A{};
class C:public A{};
class D:public C{};

TEST(TypelistTest, MostDerived){
    using mostderived_result = TypeList<A, B, C, D>::MostDerived<A>;
    ASSERT_EQ(DeMangle(typeid(mostderived_result).name()), string("D"));

    using mostderived_result2 = TypeList<>::MostDerived<A>;
    ASSERT_EQ(DeMangle(typeid(mostderived_result2).name()), string("A"));
}

TEST(TypelistTest, DerivedToFront){
    using dtof = TypeList<A,B,C,D>::DerivedToFront;
    ASSERT_EQ(dtof::toString(), string("D B C A"));
}

/*
TEST(TypelistTest, GenScatterHierarchy){
    TestInfo obj;
    string name = (static_cast<Holder<string>&>(obj)).value_;
    ASSERT_EQ(name, string("abc"));
    int i = (static_cast<Holder<int>&>(obj)).value_;
    ASSERT_EQ(i, 1);

    ASSERT_TRUE((std::is_base_of<Holder<string>, TestInfo>::value));
    //auto& value = Field_bad<string, Holder, TypeList<int, string, double, float>>(obj);
    auto& value = obj.Field_bad<string>();
    ASSERT_EQ(value.value_, string("abc"));

    auto& value_i = obj.Field_bad<int>();
    ASSERT_EQ(value_i.value_, 1);

    ASSERT_EQ(obj.Field<0>().value_, 1);
    ASSERT_EQ(obj.Field<1>().value_, string("abc"));
}*/

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

/*
TEST(TypelistTest, GenLinearHierarchy){
MyEventHandler obj;
Window window;
Button button;
obj.OnEvent(window, 1);
auto btObj = static_cast<EventHandler<Button, typename MyEventHandler::base::base::base::base>&>(obj);
btObj.OnEvent(button, 2);
}*/

/*
TEST(TypelistTest, DerivedToFront){
ASSERT_EQ( (TypeListStr<DerivedToFront< TYPELIST_4(A,B, C, D)>::Result>().str), string("D-B-C-A-NullType"));
ASSERT_EQ( (TypeListStr<DerivedToFront< TYPELIST_4(int,float,double,char)>::Result>().str), string("int-float-double-char-NullType"));
}*/