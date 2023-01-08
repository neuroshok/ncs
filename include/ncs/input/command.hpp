#ifndef INCLUDE_NCS_INPUT_COMMAND_HPP_NCS
#define INCLUDE_NCS_INPUT_COMMAND_HPP_NCS

#include <ncs/parameter.hpp>
#include <ncs/input/parameter.hpp>

#include <algorithm>
#include <exception>
#include <stdexcept>
#include <cassert>
#include <string>
#include <vector>

namespace ncs
{
    //! structured input (string -> input_command)
    class input_command
    {
    public:
        // positional value
        void add_value(std::string value)
        {
            input_parameter p{ value, std::move(value) };
            index_parameters_.emplace_back(std::move(p));
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
            assert(param_it != parameters_.end());
            return *param_it;
        }

        [[nodiscard]] const ncs::input_parameter& parameter(int index) const
        {
            assert(parameter_exist(index));
            return index_parameters_[index];
        }

        [[nodiscard]] bool parameter_exist(int index) const
        {
            return index < index_parameters_.size();
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
        std::vector<ncs::input_parameter> index_parameters_;
    };
} // ncs

#endif // INCLUDE_NCS_INPUT_COMMAND_HPP_NCS