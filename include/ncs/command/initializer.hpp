#ifndef INCLUDE_NCS_COMMAND_INITIALIZER_HPP_NCS
#define INCLUDE_NCS_COMMAND_INITIALIZER_HPP_NCS

#include <string>
#include <functional>

#define ncs_root(Command_class, ...) \
struct Command_class : ::ncs::node<__VA_ARGS__> \
{ \
    using ::ncs::node<__VA_ARGS__>::node;

#define ncs_root_() };

#define ncs_node(Name) struct:node{using node::node;
#define ncs_node_(Name) } Name{ this, #Name };

#define ncs_command(Name) struct:ncsi::command{using ncsi::command::command;
#define ncs_command_(Name, Function, Description) } Name{ this, #Name, Function, Description };

#define ncs_parameter(Name, Type, ...) ncsi::parameter<Type> Name{ this, #Name, __VA_ARGS__ };

// aliases
#define ncs_flag(Name, ...) ncs_parameter(Name, bool, __VA_ARGS__)
#define ncs_required(Name, Type, ...) ncs_parameter(Name, Type, ncs::required, __VA_ARGS__)
#define ncs_optional(Name, Type, Default, ...) ncs_parameter(Name, Type, Default, __VA_ARGS__)

namespace ncs
{
    class path;
    class cli_base;

    template<class T>
    class basic_cli;

    class node_base
    {
    public:
        virtual ncs::cli_base& get_cli() = 0;
        virtual ncs::path& path() = 0;
    };

    struct cli : public ncs::basic_cli<cli>
    {
        explicit cli(std::string name) : ncs::basic_cli<cli>(std::move(name)) {}
    };

    template<class T = ncs::cli>
    class node : public node_base
    {
    public:
        node(ncs::node<T>* node, std::string name)
            : name_{ std::move(name) }
            , cli{ node->cli }
            , path_{ node->path() }
        {
            path_.add(name_);
        }

        explicit node(ncs::basic_cli<T>& cli_)
            : cli{ static_cast<T&>(cli_) }
            , name_{ cli.module_name() }
        {
            path_.add(name_);
        }

        node(ncs::basic_cli<T>& cli_, std::string name)
            : cli{ static_cast<T&>(cli_) }
            , name_{ std::move(name) }
        {
            path_.add(name_);
        }

        [[nodiscard]] const std::string& name() const { return name_; }
        ncs::path& path() override { return path_; }

        T& cli;

    private:
        T& get_cli() override { return cli; }

        std::string name_;
        ncs::path path_;
    };

    template<class T>
    class init_command : public node_base
    {
        template<class> friend class init_parameter;
        
      public:
        template<class... Args>
        init_command(ncs::node_base* node, std::string name, Args&&... args)
            : parent_{ node }
            , command_ptr_{ node->get_cli().add(T{ ncs::path{ node->path(), std::move(name) }, std::forward<Args>(args)... }) }
        {}

        ncs::path& path() override { return parent_->path(); }
        cli_base& get_cli() override { return parent_->get_cli(); }

    private:
        ncs::node_base* parent_;
        ncs::command_ptr command_ptr_;
    };

    template<class T>
    class init_parameter
    {
      public:
        using type = T;

        init_parameter(init_command<ncs::command>* command_node, std::string name)
            : init_parameter{ command_node, std::move(name), ncs::required }
        {}

        template<class DefaultValue>
        init_parameter(init_command<ncs::command>* command_node, std::string name, DefaultValue value, std::string description = "")
            : name_{ std::move(name) }
        {
            if constexpr (std::is_same_v<T, bool>)
            {
                if constexpr (std::is_same_v<DefaultValue, ncs::required_>)
                {
                    command_node->command_ptr_->parameters_.emplace_back(ncs::parameter{ name_, bool{}, "", bool{} });
                }
                else command_node->command_ptr_->parameters_.emplace_back(ncs::parameter{ name_, bool{}, std::move(value), bool{} });
            }
            else
            {
                if constexpr (std::is_same_v<DefaultValue, ncs::required_>)
                {
                    command_node->command_ptr_->parameters_.emplace_back(ncs::parameter{ name_, T{}, std::move(description) });
                }
                else if constexpr (std::is_same_v<DefaultValue, const char*>)
                {
                    command_node->command_ptr_->parameters_.emplace_back(name_, T{}, std::move(description), std::string{ std::move(value) });
                }
                else command_node->command_ptr_->parameters_.emplace_back(name_, T{}, std::move(description), std::move(value));
            }
        }

        [[nodiscard]] const std::string& name() const { return name_; }

    private:
        std::string name_;
    };
} // ncs

//! convenient aliases
namespace ncsi
{
    using command = ncs::init_command<ncs::command>;

    template<class T>
    using parameter = ncs::init_parameter<T>;
} // ncsi

#endif // INCLUDE_NCS_COMMAND_INITIALIZER_HPP_NCS