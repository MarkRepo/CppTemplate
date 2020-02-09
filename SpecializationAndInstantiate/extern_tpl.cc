//
// Created by markfqwu on 2020-02-09.
//

#include "MyVector.h"

#include <iostream>
using namespace std;
using namespace tpl::ext;

namespace tpl{
namespace ext{
    //手动实例化
    template class MyVector<int>;
}
}

void ggg_test(double){
    cout << "ggg_test double" << endl;
}

void ggg_test2(double){
    cout <<"ggg_test2 double"<<endl;
}
