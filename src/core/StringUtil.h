#pragma once

// utility functions for working with strings

#include <string>
#include <algorithm>
#include <array>
#include <optional>

namespace util
{
    inline std::string TrimLeft(const std::string& str)
    {
        size_t begin = 0;
        for (size_t i = 0; i < str.size(); ++i)
        {
            if (str[i] != ' ' && str[i] != '\t')
            {
                begin = i;
                break;
            }
        }

        return str.substr(begin);
    }

    inline std::string TrimRight(const std::string& str)
    {
        size_t end = 0;
        for (size_t i = str.size(); i != 0; --i)
        {
            if (str[i] != ' ' && str[i] != '\t')
            {
                end = i;
                break;
            }
        }
        return str.substr(0, end);
    }

    inline std::string Trim(std::string& str)
    {
        
        return TrimRight(TrimLeft(str));
    }
}