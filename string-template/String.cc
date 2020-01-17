//
// Created by markfqwu on 2020-01-17.
//

#include "String.h"

#include <iostream>

using namespace wfq;

template <typename C>
String<C>::String() :sz(0), ptr(ch){
    std::cout<<"in String constructor" << std::endl;
}

template <typename C>
String& String<C>::operator+=(C c) {
    //todo
    std::cout<<"in operatore+= c is " << c << std::endl;
    return *this;
}

