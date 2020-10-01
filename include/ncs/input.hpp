#ifndef INCLUDE_NCS_INPUT_HPP_NCS
#define INCLUDE_NCS_INPUT_HPP_NCS

#include <exception>
#include <string>
#include <vector>

#include <iostream>

namespace ncs
{
    struct input_parameter
    {
        std::string name;
        std::string value;
        bool required = false;
    };

    class input_command
    {
    public:
        void add_value(std::string value)
        {
            std::cout << "\n__add value " << value;
            input_parameter p{ std::move(value), std::move(value), true };
            parameters_.emplace_back(std::move(p));
        }

        // use default value
        void add_option(std::string name)
        {
            std::cout << "\n__add_option default" << name;
            input_parameter p{ std::move(name), "", false };
            parameters_.emplace_back(std::move(p));
        }

        void add_option(std::string name, std::string value)
        {
            std::cout << "\n__add_option " << name << " " << value;
            input_parameter p{ std::move(name), std::move(value), false };
            parameters_.emplace_back(std::move(p));
        }

        template<class Parameter>
        typename Parameter::type operator[](const Parameter& parameter) const
        {
            auto param_it = std::find_if(parameters_.begin(), parameters_.end(), [&](const auto& ip){ return parameter.name() == ip.name; });
            if (param_it == parameters_.end()) throw std::logic_error("parameter not found, validation error");
            return param_it->value;
        }

    private:
        std::vector<input_parameter> parameters_;
    };
} // ncs

#endif // INCLUDE_NCS_INPUT_HPP_NCS