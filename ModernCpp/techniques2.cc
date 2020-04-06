//
// Created by markfqwu on 2020-03-14.
//

#include "gtest/gtest.h"
#include "ModernCpp/techniques2.h"

using namespace modern::tech2;

TEST(TechTest, static_checker){
    void * p = nullptr;
    //char c = safe_reinterpret_cast<char>(p);
    int* a = safe_reinterpret_cast<int*>(p);

}

TEST(TechTest, LocalClass){
    int a = 5;
    MakeAdapter(CallObjType<int>{}, a)->Fun();
}

TEST(TechTest, Int2Type){
    int a = 5;
    NiftyContainer<int, false>().DoSomething(&a);
    NiftyContainer<int, true>().DoSomething(&a);
}

TEST(TechTest, Type2Type){
    int a = 5;
    string* pStr = Create("Hello", Type2Type<string>());
    Widget* pw = Create(100, Type2Type<Widget>());
}

TEST(TechTest, Conversion){
    cout << Conversion<double, int>::exists << "  " <<
    Conversion<char, char*>::exists << "  " <<
    Conversion<size_t, vector<int>>::exists << "  " << endl;
}

TEST(TechTest, supersub){
    if(SUPERSUBCLASS(int, double)){
        cout << "double is sub class of int" << endl;
    }else{
        cout << "double is not sub class of int " << endl;
    }
}