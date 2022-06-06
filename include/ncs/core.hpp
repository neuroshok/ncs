#ifndef INCLUDE_NCS_CORE_HPP_NCS
#define INCLUDE_NCS_CORE_HPP_NCS

#include <iostream>
#include <string>
#include <format>
#include <color.hpp>

namespace ncs
{
    class core
    {
    public:
        core() = default;

    template<typename... Args>
    void log(std::string_view message, Args&&... args) const
    {
        std::cout<< hue::blue << "[ncs] " << hue::reset << std::vformat(message, std::make_format_args(std::forward<Args>(args)...)) << std::endl;
    }
    private:
    };
} // ncs

#endif // INCLUDE_NCS_CORE_HPP_NCS