#ifndef INCLUDE_NCS_PARAMETER_HPP_NCS
#define INCLUDE_NCS_PARAMETER_HPP_NCS

#include <optional>
#include <string>
#include <variant>

namespace ncs
{
    using parameter_type = std::variant<std::string, int, bool>;
    struct parameter
    {
        template<class TypeTag>
        parameter(std::string name, TypeTag tag, std::string description)
            : name_{ std::move(name) }
            , description_{ std::move(description) }
            , default_value_{ std::move(tag) }
            , has_default_value_{ false }
        {}

        template<class V, class TypeTag>
        parameter(std::string name, TypeTag tag, std::string description, V default_value)
            : name_{ std::move(name) }
            , description_{ std::move(description) }
            , default_value_{ std::move(default_value) }
            , has_default_value_{ true }
        {}

        [[nodiscard]] const std::string& name() const { return name_; }
        [[nodiscard]] bool required() const { return !has_default_value(); }
        [[nodiscard]] const std::string& description() const { return description_; }
        [[nodiscard]] const parameter_type& default_value() const { return default_value_; }

        [[nodiscard]] bool has_default_value() const
        {
            return has_default_value_;
        }

        static std::string to_string(const parameter_type& param)
        {
            return std::visit(
            [](auto&& v) {
                using T = std::decay_t<decltype(v)>;
                if constexpr (std::is_same_v<T, bool>)
                    return v ? std::string("true") : std::string("false");
                else if constexpr (std::is_arithmetic_v<T>)
                    return std::to_string(v);
                else if constexpr (std::is_same_v<T, std::string>)
                    return v;
                else
                    return std::string();
            },
            param);
        }

    private:
        std::string name_;
        std::string description_;
        parameter_type default_value_;
        bool has_default_value_;
    };
} // ncs

#endif // INCLUDE_NCS_PARAMETER_HPP_NCS