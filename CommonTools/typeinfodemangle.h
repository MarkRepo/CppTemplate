//
// Created by markfqwu on 2020-01-28.
//

#ifndef CPPTEMPLATE_TYPEINFODEMANGLE_H
#define CPPTEMPLATE_TYPEINFODEMANGLE_H

#include <string>

using std::string;

namespace tpl{
namespace common{

string DeMangle(string);
void   DeMangleToCout(string);

}//namespace common
}//namespace tpl

#endif //CPPTEMPLATE_TYPEINFOSTR_H
