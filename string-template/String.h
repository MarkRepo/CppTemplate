//
// Created by markfqwu on 2020-01-17.
//

#ifndef CPPTEMPLATE_STRING_H
#define CPPTEMPLATE_STRING_H
#include <iostream>

namespace wfq {

    template<typename C>
    class String{
    public:
        String();
        explicit String(const C*);
        String operator=(const String&);
        //...
        C& operator[](int n){return ptr[n];}
        String& operator+=(C c);
        //...
    private:
        static const int short_max = 15;
        int sz;
        C* ptr;
        C ch[short_max];
    };

    template <typename C>
    String<C>::String() :sz(0), ptr(ch){
        std::cout<<"in String constructor" << std::endl;
    }

    template <typename C>
    String<C>& String<C>::operator+=(C c) {
        //todo
        std::cout<<"in operatore+= c is " << c << std::endl;
        return *this;
    }

}//namespace template
#endif //CPPTEMPLATE_STRING_H
