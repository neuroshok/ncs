#ifndef INCLUDE_NCS_COMMAND_HPP_NCS
#define INCLUDE_NCS_COMMAND_HPP_NCS

#include <ncs/input.hpp>
#include <ncs/parameter.hpp>

#include <string>
#include <functional>

namespace ncs
{
    class path;
    class cli_base;

    template<class T>
    class cli;

    template<class T>
    class init;
    
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
    class init
    {
      public:
        template<class... Args>
        init(ncs::node_base* node, std::string name, Args&&... args)
        {
            ncs::path path{ node->path() };
            path.add(std::move(name));
            node->get_cli().add(T{ std::move(path), std::forward<Args>(args)... });
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

        void add(std::string node_name)
        { 
            str_path_ = str_path_ + node_name + " ";
            nodes_.emplace_back(std::move(node_name));
        }
        const node_type& node(unsigned int n) const { return nodes_[n]; }
        const node_type& last() const { return nodes_.back(); }
        const std::string& str() const { return str_path_; }
        std::size_t size() const { return nodes_.size(); }
        const std::vector<node_type>& data() const { return nodes_; }
        
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
} // ncs

#endif // INCLUDE_NCS_COMMAND_HPP_NCS