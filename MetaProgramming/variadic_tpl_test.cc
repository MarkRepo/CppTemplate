//
// Created by markfqwu on 2020-02-06.
//
#include "gtest/gtest.h"
#include "MetaProgramming/variadic_tpl.h"

#include <string>

using namespace tpl::variadic;
using std::string;

TEST(VariadicTest, Tuple){
    class FO{};
    Tuple<double, string, int, char> {3.14, string{"Bob"}, 127, 'c'};

    //测试空基类优化
    typedef Tuple<int*, int*> T0;
    typedef Tuple<int*,FO> T1;
    typedef Tuple<int*, FO, FO> T2;
    cout << "T0 size " << sizeof(T0) << endl;
    cout << "T1 size " << sizeof(T1) << endl;
    cout << "T2 size " << sizeof(T2) << endl;
}

TEST(VariadicTest, TuplePrintElement){
    Tuple<double, int, char> x{1.1, 42, 'a'};
    cout << x << endl;

    cout << Tuple<double,int,int,int>{1.2, 3,5,7}<<endl;
    cout << Tuple<double,int,int>{1.2,3,5} << endl;
    cout << Tuple<double,int>{1.2,3} << endl;
    cout << Tuple<double>{1.2} << endl;
    cout << Tuple<>{} << endl;
}

TEST(VariadicTest, TupleGet){
    Tuple<double,int, char> x{1.1, 42, 'c'};
    cout << "{" << get<0>(x) << "," << get<1>(x) << "," << get<2>(x) << "}" << endl;
    const Tuple<double,int,char>xx{2.2, 84, 'd'};
    char cc = get<2>(xx);
    cout << "xx: "<< xx << endl;
}

TEST(VariadicTest, TupleMakeTuple){
    auto xxx = make_tuple(1.2, 3, 'x', 1223);
    cout << "make_tuple xxx: " << xxx <<endl;
}

TEST(VariadicTest, printf){
    string a = "abcdefg";
    int b = 6;
    double d = 7.8;

    tpl::variadic::printf("string %s, int %d, double %f", a, b, d);
}

TEST(VariadicTest, exp){
    int a = 0;
    string s = "abcd";
    exp_call(a, s);
}

void g0() { cout << "g0()\n"; }

template<typename T>
void g1(const T& t)
{
    cout << "g1(): " << t << '\n';
}

void g1d(double t) { cout << "g1d(): " << t << '\n'; }

template<typename T, typename T2>
void g2(const T& t, T2&& t2)
{
    cout << "g2(): " << t << ' ' << t2 << '\n';
}

TEST(VariadicTest, call){

    call(g0);
    //call(g1); // error : too few arguments
    call(g1<int>,1);//必需指明传递函数模版的哪个特例化版本，因为call不能从其他实参的类型推断
    call(g1<const char*>,"hello");
    call(g1<double>,1.2);
    call(g1d,1.2);
    //call(g1d,"No way!");// error: wrong argument type for g1d()
    //call(g1d,1.2,"I can't count");// error: too many arguments for g1d()
    call(g2<double,string>,1,"world!");
    int i = 99;// testing with lvalues
    const char* p = "Trying";
    call(g2<double,string>,i,p);
    call([](){ cout <<"l1()\n"; });
    call([](int i){ cout <<"l0(): " << i << "\n";},17);
    call([i](){ cout <<"l1(): " << i << "\n"; });
}

template <typename... T>
using std_tuple = tpl::variadic::tuple<T...>;

TEST(VariadicTest, tuple){

    string s1 = "abcd";
    std_tuple<int, char, double, float, string> t{1, 'c', 2.2, 1.1, s1};
    auto i = t.head();
    auto tail = t.tail();
    auto tail_head = tail.head();
    cout << "tuple{1, 'c', 2.2, 1.1, string(\"abcd\"}'s head is " << i << endl;
    cout << "tuple t tailf_head " << tail_head << endl;

    string s = "Hello";
    vector<int> v {1,2,3,4};
    std_tuple<string, vector<int>, double> tt{s, v, 1.2};
    auto x = std_make_tuple(s, v, 1.2);
    std_make_tuple("hello", 1.2);
}

TEST(VariadicTest, const_test){
    string s = "Hello";
    const_test<string> ct{s};
    make_const_test(s);
}

TEST(VariadicTest, std_get){
    std_tuple<int, char, double, string, bool, string> st{1, 'a', 1.1, string("aaa"), false, string("bbb")};
    auto b = std_get<5>(st);
    cout << "std_get 4 element is " << b << endl;
}

TEST(VariadicTest, print_tuple){
    cout << std_make_tuple() << '\n';
    cout << std_make_tuple("One meatball!") << '\n';
    cout << std_make_tuple(1,1.2,"Tail!") << '\n';
    std_tuple<int, char, double, string, bool, string> st{1, 'a', 1.1, string("aaa"), false, string("bbb")};
    cout << st << endl;
}


using echoType = void(MFB::*)(string, int, int);

TEST(MFTest, echo){
    MF mf(95.5);
    string name = "markfqwu";
    int a = 5;
    echoType pEcho = &MF::echo;
    MF_test(&mf, pEcho, name, 30);
    MF_test2(30);
}

TEST(FstreamTest, fstream){
    fstream_test("/tmp/logreplay_main_port");
}