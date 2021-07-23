#ifndef INCLUDE_NCS_INPUT_PARAMETER_HPP_NCS
#define INCLUDE_NCS_INPUT_PARAMETER_HPP_NCS

#include <string>

namespace ncs
{
    struct input_parameter
    {
        std::string name;
        std::string value;
        bool is_flag = false;
    };
} // ncs

#endif // INCLUDE_NCS_INPUT_PARAMETER_HPP_NCS
