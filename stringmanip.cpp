#include "stringmanip.h"

#include <string>
#include <sstream>
#include <algorithm>
#include <iostream>

using std::string;
using std::istringstream;
using std::find_if;
using std::isspace;
using std::boolalpha;
using std::invalid_argument;

void str_trim(string& str)
{
    auto cmpfunc = [](unsigned char ch){return !std::isspace(ch);};
    str.erase(str.begin(), std::find_if(str.begin(), str.end(), cmpfunc));
    str.erase(std::find_if(str.rbegin(), str.rend(), cmpfunc).base(), str.end());
}

bool str2bool(string& str)
{
    auto result = false;

    istringstream is(str);
    
    // first try simple integer conversion
    is >> result;

    if (is.fail())
    {
        // simple integer failed; try boolean
        is.clear();
        is >> std::boolalpha >> result;
    }

    if (is.fail())
    {
        throw invalid_argument(str.append(" is not convertable to bool"));
    }

    return result;
}
