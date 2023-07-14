#ifndef INCLUDE_NCS_UTILITY_HPP_NCS
#define INCLUDE_NCS_UTILITY_HPP_NCS

#include <string>

namespace ncs
{
    //template<class T>
    inline void str_replace(std::string& str, const std::string& from, const std::string& to)
    {
        if (from.empty()) return;
        size_t start_pos = 0;
        while ((start_pos = str.find(from, start_pos)) != std::string::npos)
        {
            str.replace(start_pos, from.length(), to);
            start_pos += to.length();
        }
    }
} // ncs

#endif // INCLUDE_NCS_UTILITY_HPP_NCS