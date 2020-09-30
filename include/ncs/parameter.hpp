#ifndef INCLUDE_NCS_PARAMETER_HPP_NCS
#define INCLUDE_NCS_PARAMETER_HPP_NCS

#include <string>
#include <variant>
#include <optional>

namespace ncs
{
    using parameter_type = std::variant<std::string, int, bool>;
    struct parameter
    {
        parameter(std::string name, std::string description = "")
            : name_{ std::move(name) }
            , required_{ true }
            , description_{ std::move(description) }
        {}

        template<class V>
        parameter(std::string name, std::string description, V default_value)
            : name_{ std::move(name) }
            , required_{ false }
            , description_{ std::move(description) }
            , default_value_{ std::move(default_value) }
        {}

        const std::string& name() const { return name_; }
        bool required() const { return required_; }
        const std::string& description() const { return description_; }
        const parameter_type& default_value() const { return *default_value_; }

        static std::string to_string(const parameter_type& param)
        {
            return std::visit([](auto&& v) {
            using T = std::decay_t<decltype(v)>;
            if constexpr (std::is_arithmetic_v<T>)
                return std::to_string(v);
            else if constexpr (std::is_same_v<T, std::string>)
                return v;
            else return "";
            }, param);
        }
  
    private:
        std::string name_;
        bool required_;
        std::string description_;
        std::optional<parameter_type> default_value_;
    };
} // ncs

#endif // INCLUDE_NCS_PARAMETER_HPP_NCS