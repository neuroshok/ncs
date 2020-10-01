#ifndef INCLUDE_NCS_COMMAND_INITIALIZER_HPP_NCS
#define INCLUDE_NCS_COMMAND_INITIALIZER_HPP_NCS

#include <string>
#include <functional>

#define ncs_node(Name) struct:node{using node::node;
#define ncs_node_(Name) } Name{ this, #Name };

#define ncs_command(Name) struct:ncsi::command{using ncsi::command::command;
#define ncs_command_(Name, Function, Description) } Name{ this, #Name, Function, Description };

#define ncs_parameter(Type, Name, ...) ncsi::parameter<Type> Name{ this, #Name, __VA_ARGS__ };


namespace ncs
{
    class path;
    class cli_base;

    template<class T>
    class cli;

    class node_base
    {
    public:
        virtual ncs::cli_base& get_cli() = 0;
        virtual ncs::path& path() = 0;
    };

    template<class T>
    class node : public node_base
    {
        template<class T> friend class init;

    public:
        node(ncs::node<T>* node, std::string name)
            : name_{ std::move(name) }
            , cli{ node->cli }
            , path_{ node->path() }
        {
            path_.add(name_);
        }

        node(ncs::cli<T>& cli_, std::string name)
            : name_{ std::move(name) }
            , cli{ static_cast<T&>(cli_) }
        {
            path_.add(name_);
        }

        const std::string& name() const { return name_; }
        ncs::path& path() override { return path_; }

        T& get_cli() override { return cli; }

        T& cli;

    private:
        std::string name_;
        ncs::path path_;
    };

    template<class T>
    class init_command : public node_base
    {
        template<class T> friend class init_parameter;
        
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

        template<class... Args>
        init_parameter(init_command<ncs::command>* command_node, std::string name, Args&&... args)
            : name_{ std::move(name) } 
        {
            command_node->command_ptr_->parameters_.emplace_back(name_, std::forward<Args>(args)...);
        }

        const std::string& name() const { return name_; }

    private:
        std::string name_;
    };
} // ncs

//! conveniant aliases
namespace ncsi
{
    using command = ncs::init_command<ncs::command>;

    template<class T>
    using parameter = ncs::init_parameter<T>;
} // ncsi

#endif // INCLUDE_NCS_COMMAND_INITIALIZER_HPP_NCS