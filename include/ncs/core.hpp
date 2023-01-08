#ifndef INCLUDE_NCS_CORE_HPP_NCS
#define INCLUDE_NCS_CORE_HPP_NCS

#include <color.hpp>

#include <ncs/command.hpp>
#include <ncs/module.hpp>
#include <ncs/type.hpp>
#include <format>

#include <iostream>
#include <string>
#include <vector>

namespace ncs
{
    class core
    {
    public:
        core(int argc, const char* argv[]) noexcept;

        // void process(int argc, const char* argv[]);
        // void process(std::string_view);
        void process();
        bool search(const ncs::commands_view& commands);

        //void add_commands();

        template<class Module>
        void add_module()
        {
            modules_.emplace_back(std::make_unique<Module>(*this));
        }

        template<typename... Args>
        void log(std::string_view message, Args&&... args) const
        {
            std::cout<< hue::blue << "[ncs] " << hue::reset << std::vformat(message, std::make_format_args(std::forward<Args>(args)...)) << std::endl;
        }

    private:
        std::string module_name_;
        std::vector<std::string> input_parameters_;
        std::vector<std::unique_ptr<ncs::module>> modules_;
    };
} // ncs

#endif // INCLUDE_NCS_CORE_HPP_NCS