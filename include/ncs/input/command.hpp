#ifndef INCLUDE_NCS_INPUT_COMMAND_HPP_NCS
#define INCLUDE_NCS_INPUT_COMMAND_HPP_NCS

#include <ncs/parameter.hpp>
#include <ncs/input/parameter.hpp>

#include <algorithm>
#include <exception>
#include <stdexcept>
#include <string>
#include <vector>

namespace ncs
{
    class input_command
    {
    public:
        void add_value(std::string value)
        {
            input_parameter p{ value, std::move(value) };
            parameters_.emplace_back(std::move(p));
        }

        // use default value
        void add_parameter(std::string name)
        {
            input_parameter p{ std::move(name), "" };
            parameters_.emplace_back(std::move(p));
        }

        void add_parameter(std::string name, std::string value)
        {
            input_parameter p{ std::move(name), std::move(value) };
            parameters_.emplace_back(std::move(p));
        }

        void add_flag_parameter(std::string name)
        {
            input_parameter p{ std::move(name), "", true };
            parameters_.emplace_back(std::move(p));
        }

        [[nodiscard]] const ncs::input_parameter& parameter(const std::string& name) const
        {
            auto param_it = std::find_if(parameters_.begin(), parameters_.end(), [&](const auto& ip){ return name == ip.name; });
            if (param_it == parameters_.end()) throw std::logic_error("parameter not found");
            return *param_it;
        }

        [[nodiscard]] bool parameter_exist(const std::string& name) const
        {
            auto param_it = std::find_if(parameters_.begin(), parameters_.end(), [&](const auto& ip) { return name == ip.name; });
            return param_it != parameters_.end();
        }

        [[nodiscard]] const std::string& operator[](const ncs::parameter& param) const
        {
            return parameter(param.name()).value;
        }

    private:
        std::vector<ncs::input_parameter> parameters_;
    };
} // ncs

#endif // INCLUDE_NCS_INPUT_COMMAND_HPP_NCS