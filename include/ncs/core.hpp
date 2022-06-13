#ifndef INCLUDE_NCS_CORE_HPP_NCS
#define INCLUDE_NCS_CORE_HPP_NCS

#include <color.hpp>

#include <iostream>
#include <string>
#include <format>
#include <vector>

namespace ncs
{
    class core
    {
    public:
        core(int argc, const char* argv[]) noexcept;

        void process();

        template<typename... Args>
        void log(std::string_view message, Args&&... args) const
        {
            std::cout<< hue::blue << "[ncs] " << hue::reset << std::vformat(message, std::make_format_args(std::forward<Args>(args)...)) << std::endl;
        }
    private:
        std::string module_;
        std::vector<std::string> raw_parameters_;
    };
} // ncs

#endif // INCLUDE_NCS_CORE_HPP_NCS