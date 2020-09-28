#ifndef INCLUDE_NCS_COMMAND_HPP_NCS
#define INCLUDE_NCS_COMMAND_HPP_NCS

#include <ncs/parameter.hpp>

#include <string>
#include <functional>

namespace ncs
{
    template<class T>
    class init;

    class cli_base;

    template<class T>
    class cli;
    
    class node_base
    {
    public:
        virtual ncs::cli_base& get_cli() = 0;
    };

    template<class T>
    class node : public node_base
    {
        template<class T> friend class init;

    public:
        node(ncs::node<T>* node, std::string name)
            : name_{ std::move(name) }
            , cli{ node->cli }
        {}

        node(ncs::cli<T>& cli_, std::string name)
            : name_{ std::move(name) }
            , cli{ static_cast<T&>(cli_) }
        {}

        const std::string& name() const { return name_; }

        T& get_cli() override { return cli; }

        T& cli;

    private:
        std::string name_;
    };

    template<class T>
    class init
    {
      public:
        template<class... Args>
        init(ncs::node_base* node, Args&&... args)
        {
            node->get_cli().add(T{ std::forward<Args>(args)... });
        }
    };

    class path
    {
        using node_type = std::string;
        
    public:
        template<class... Ts>
        path(Ts... ts)
            : str_path_{ (std::string{} + ... + (ts + std::string(" "))) }
            , nodes_{ std::move(ts)... }
        {}

        const node_type& last() const { return nodes_.back(); }
        const std::string& str() const { return str_path_; }
        
    private:
        std::string str_path_;
        std::vector<node_type> nodes_;
    };

    class command
    {
    public:
        using function_type = std::function<void()>;

        template<class... Ts>
        command(ncs::path path, function_type fn, std::string description = "", Ts&&... ts)
            : path_{ std::move(path) }
            , description_{ std::move(description) }
            , parameters_{ std::forward<Ts>(ts)... }
            , function_{ std::move(fn) }
        {}

        void exec()
        {
            function_();
        }

        const std::string& name() const { return path_.last(); }
        const std::string& description() const { return description_; }
        const std::string& str_path() const { return path_.str(); }
        const std::vector<ncs::parameter>& parameters() const { return parameters_; }

    private:
        ncs::path path_;
        std::string description_;
        std::vector<ncs::parameter> parameters_;
        function_type function_;
    };


} // ncs

//command::add( "nc:project", "add", "vcs"_bool = false, "vcs"_bool = true );




/*
class nc
{
    void project_add(const std::string& name)
}

struct project_
{
    ngl::command<required<std::string>, int> test{ "test", "name", "size" }
    ngl::command<std::string, template_> add{ "add",  }
} project{this};

nc project test raz 2
nc project add raz -template.type:cpp
nc project add raz -template<cpp>
nc project add raz -template: .type:cpp .vcs:git
nc project config -vcs
// if param exist && is_bool, autoactivate (-vcs:true)
*/

#endif // INCLUDE_NCS_COMMAND_HPP_NCS