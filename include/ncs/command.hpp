#ifndef INCLUDE_NCS_COMMAND_HPP_NCS
#define INCLUDE_NCS_COMMAND_HPP_NCS

#include <ncs/input.hpp>
#include <ncs/parameter.hpp>
#include <ncs/path.hpp>

#include <string>
#include <functional>

namespace ncs
{
    class command
    {
        template<class T> friend class init_parameter;

    public:
        using function_type = std::function<void()>;

        template<class... Ts>
        command(ncs::path path, function_type fn, std::string description = "", Ts&&... ts)
            : path_{ std::move(path) }
            , description_{ std::move(description) }
            , parameters_{ std::forward<Ts>(ts)... }
            , function_{ std::move(fn) }
        {}

        void exec() const
        {
            function_();
        }

        const std::string& name() const { return path_.last(); }
        const std::string& description() const { return description_; }
        const ncs::path& path() const { return path_; }
        const std::string& str_path() const { return path_.str(); }
        const std::vector<ncs::parameter>& parameters() const { return parameters_; }

    private:
        ncs::path path_;
        std::string description_;
        std::vector<ncs::parameter> parameters_;
        function_type function_;
    };

    using command_ptr = ncs::command*;
} // ncs

#endif // INCLUDE_NCS_COMMAND_HPP_NCS