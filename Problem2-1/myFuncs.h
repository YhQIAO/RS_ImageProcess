#pragma once

#include<vector>
#include <string>

using namespace std;

// 按符号分割字符串，存到vector里，网上白嫖的代码
vector<string> split_str(string str, string pattern)
{
    string::size_type pos;
    vector<string> result;
    str += pattern;
    size_t size = str.size();
    for (size_t i = 0; i < size; i++)
    {
        pos = str.find(pattern, i);
        if (pos < size)
        {
            string s = str.substr(i, pos - i);
            result.push_back(s);
            i = pos + pattern.size() - 1;
        }
    }
    return result;
}