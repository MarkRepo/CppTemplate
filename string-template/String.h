//
// Created by markfqwu on 2020-01-17.
//

#ifndef CPPTEMPLATE_STRING_H
#define CPPTEMPLATE_STRING_H

namespace wfq {

    template<typename C>
    class String{
    public:
        String();
        explicit String(const C*);
        String operator=(const string&);
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

}//namespace template
#endif //CPPTEMPLATE_STRING_H
