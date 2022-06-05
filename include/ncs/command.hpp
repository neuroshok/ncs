#ifndef INCLUDE_NCS_COMMAND_HPP_NCS
#define INCLUDE_NCS_COMMAND_HPP_NCS

#include <ncs/parameter.hpp>
#include <ncs/parameters.hpp>
#include <ncs/path.hpp>

#include <string>
#include <functional>

namespace ncs
{
    class command
    {
        template<class T> friend class init_parameter;

    public:
        using function_type = std::variant<
        std::function<void()>
        , std::function<void(const ncs::parameters&)>>;

        template<class... Ts>
        command(ncs::path path, function_type fn, std::string description = "", Ts&&... ts)
            : path_{ std::move(path) }
            , description_{ std::move(description) }
            , parameters_{ std::forward<Ts>(ts)... }
            , function_{ std::move(fn) }
        {}

        void exec(const ncs::parameters& parameters) const
        {
            std::visit([&parameters](auto&& fn) {
            using T = std::decay_t<decltype(fn)>;
            if constexpr (std::is_same_v<T, std::function<void()>>)
                fn();
            else fn(parameters);
            }, function_);
        }

        [[nodiscard]] const std::string& name() const { return path_.last(); }
        [[nodiscard]] const std::string& description() const { return description_; }
        [[nodiscard]] const ncs::path& path() const { return path_; }
        [[nodiscard]] const std::string& str_path() const { return path_.str(); }
        [[nodiscard]] const std::vector<ncs::parameter>& parameters() const { return parameters_; }

    private:
        ncs::path path_;
        std::string description_;
        std::vector<ncs::parameter> parameters_;
        function_type function_;
    };

    using command_ptr = ncs::command*;
} // ncs

#endif // INCLUDE_NCS_COMMAND_HPP_NCS