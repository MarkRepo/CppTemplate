//
// Created by markfqwu on 2020-01-29.
//

#include "gtest/gtest.h"
#include "MetaProgramming/meta.h"

using namespace tpl::meta;

TEST(MetaTest, Obj_holder){
    tpl::meta::Obj_holder_test();
}

TEST(MetaTest, Holder){
    tpl::meta::Holder_test();
}

TEST(MetaTest, GetUnsignedIntegerT){
    tpl::meta::GetUnsignedIntegerT_test();
}

TEST(MetaTest, PodCopyTest){
    tpl::meta::pod_copy_test();
}

TEST(MetaTest, BigObjHolderTest){
    tpl::meta::big_obj_holder_test();
}

TEST(MetaTest, MyConditionalTest){
    int a = my_conditional_test<int>(10);
    ASSERT_EQ(a, 100);
    int b = my_conditional_test<std::string>(10);
    ASSERT_EQ(b, 1000);
}

TEST(MetaTest, Select1){
    Select1<0, char, int, long> a;
    string char_type_str = DeMangle(typeid(a).name());
    ASSERT_EQ(char_type_str, "char");
    Select1<1, char, int, long> b;
    string int_type_str = DeMangle(typeid(b).name());
    ASSERT_EQ(int_type_str, "int");
}

TEST(MetaTest, Select){
    Select<0, char, int, long, double> a;
    string char_str = DeMangle(typeid(a).name());
    ASSERT_EQ(char_str, "char");
    cout << "char_str "<< char_str << endl;

    Select<1, char, int, long, double> b;
    string int_str = DeMangle(typeid(b).name());
    ASSERT_EQ(int_str, "int");
    cout << "int_str " << int_str << endl;

    Select<2, char, int, long, double, char, int, double> c;
    string long_str = DeMangle(typeid(c).name());
    ASSERT_EQ(long_str, "long");
    cout << "long_str " <<long_str << endl;
}

TEST(MetaTest, facTest){
    constexpr int x5  = fac<5>();
    ASSERT_EQ(x5, 120);

    constexpr int y5 = Fac<5>::value;
    ASSERT_EQ(y5, 120);
}

TEST(MetaTest, SmartPointer){
    Smart_pointer<double> d{new double};
    Smart_pointer<std::string> s{new string()};

    //auto d0 = d->size();
    auto s0 = s->size();
}

TEST(MetaTest, MyVector){
    MyVector<int> v_i(10, 20);
    string b{}; string e{};
    MyVectorEnable<int> v_e(10,20);
    MyVectorEnable<std::string> v_s(b, e);
    MyVectorEnableArg<int> v_a(10, 20);
    MyVectorEnableArg<std::string> v_a_s(b, e);
}

TEST(MetaTest, Has_f){
    X<int> x_int;
    X<double> x_double;
    X<std::string> x_s;
    int i = 5;
    double d = 0.1;
    string s{};
    x_int.use_f(i);
    x_double.use_f(d);
    x_s.use_f(s);
}