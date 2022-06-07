#ifndef INCLUDE_NCS_PARAMETERS_HPP_NCS
#define INCLUDE_NCS_PARAMETERS_HPP_NCS

#include <ncs/parameter.hpp>

#include <algorithm>
#include <unordered_map>
#include <stdexcept>

namespace ncs
{
    //! validated and typed parameters from input
    class parameters
    {
        struct param
        {
            explicit param(parameter_type v, bool input_exist = true) : value{ std::move(v) }, exist{ input_exist } {}
            parameter_type value;
            bool exist = false;
        };

        template<class Parameter>
        const param& get(const Parameter& p) const
        {
            auto param_it = std::find_if(parameters_.begin(), parameters_.end(), [&p](auto&& ip) { return ip.first == p.name(); });
            if (param_it == parameters_.end()) throw std::logic_error("parameter not found");

            return param_it->second;
        }

    public:
        bool add(const ncs::parameter& parameter, const std::string& value)
        {
            try
            {
                std::visit(
                [this, &parameter, &value](auto&& v) {
                    using ParameterType = std::decay_t<decltype(v)>;
                    if constexpr (std::is_same_v<ParameterType, bool>)
                    {
                        if (value == "true" || value == "1")
                            parameters_.emplace(parameter.name(), param{ true });
                        else if (value == "false" || value == "0")
                            parameters_.emplace(parameter.name(), param{ false });
                    }
                    else if constexpr (std::is_same_v<ParameterType, int>)
                        parameters_.emplace(parameter.name(), param{ std::stoi(value)});
                    else parameters_.emplace(parameter.name(), param{ value });
                },
                parameter.default_value());
            } catch (const std::exception&) { return false; }

            return true;
        }

        void add_default(const ncs::parameter& parameter, bool input_exist = true)
        {
            parameters_.emplace(parameter.name(), param{ parameter.default_value(), input_exist });
        }

        template<class Parameter>
        bool has(const Parameter& p) const
        {
            return get(p).exist;
        }
        template<class Parameter>
        typename Parameter::type operator[](const Parameter& p) const
        {
            if constexpr (std::is_same_v<typename Parameter::type, bool>)
            {
                return get(p).exist;
            }
            else return std::get<typename Parameter::type>(get(p).value);
        }

        template<class T = std::string>
        const T& value(const std::string& name) const
        {
            return std::get<T>(parameters_.at(name).value);
        }

    private:
        std::unordered_map<std::string, parameters::param> parameters_;
    };

    //! experimental
    template<class T>
    struct named_parameters
    {
        named_parameters(T& p, const ncs::parameters& params) : param{ p }, params_{ params } {}

        template<class Parameter>
        bool has(const Parameter& p) const
        {
            return params_.has(p);
        }

        template<class Parameter>
        typename Parameter::type operator[](const Parameter& p) const
        {
            return params_[p];
        }

        const ncs::parameters& params_;
        T& param;
    };
} // ncs

#endif // INCLUDE_NCS_PARAMETERS_HPP_NCS