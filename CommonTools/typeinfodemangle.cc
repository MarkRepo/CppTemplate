//
// Created by markfqwu on 2020-01-28.
//

#include "typeinfodemangle.h"

#include <stdio.h>
#include <iostream>

using std::cout;
using std::endl;

static constexpr int MAX = 256;

string tpl::common::DeMangle(string mangleStr) {
    FILE* fp = nullptr;
    auto status = 0;
    char recvBuf[MAX] = {0};
    snprintf(recvBuf, MAX-1, "c++filt %s", mangleStr.data());
    fp = popen(recvBuf, "r");
    if(fp == nullptr){
        std::cout << "popen error, common is " << recvBuf << std::endl;
        return string();
    }
    if(fgets(recvBuf, MAX, fp) != nullptr){
        if(recvBuf[strlen(recvBuf)-1] == '\n')
            recvBuf[strlen(recvBuf)-1] = '\0';
        return string(recvBuf);
    }
    return string();
}

void tpl::common::DeMangleToCout(string mangleStr) {
    string demangle_str = DeMangle(mangleStr);
    std::cout << demangle_str << endl;
}