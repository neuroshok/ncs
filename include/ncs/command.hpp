#include <string>
#include <variant>
#include <vector>
#include <functional>
#include <optional>

namespace ncs
{
    template<class T>
    class init;

    class cli;

    class node
    {
        template<class T> friend class init;
    public:
        node(ncs::node* node, std::string name)
            : name_{ std::move(name) }
            , cli_{ node->cli_ }
        {}

        node(ncs::cli& cli, std::string name)
            : name_{ std::move(name) }
            , cli_{ cli }
        {}

        const std::string& name() const { return name_; }

    private:
        std::string name_;
        ncs::cli& cli_;

    };

    template<class T>
    class init
    {
      public:
        template<class... Args>
        init(ncs::node* node, Args&&... args)
        {
            node->cli_.add(T{ std::forward<Args>(args)... });
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


    using parameter_type = std::variant<std::string, int, bool>;
    struct parameter
    {
        parameter(std::string name, std::string description = "")
            : name_{ std::move(name) }
            , required_{ true }
            , description_{ std::move(description) }
        {}

        template<class T>
        parameter(std::string name, std::string description, T default_value)
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

        const std::string& name() const { return path_.last(); }
        const std::string& description() const { return description_; }
        const std::string& str_path() const { return path_.str(); }
        const std::vector<ncs::parameter>& parameters() const { return parameters_; }

    private:
        ncs::path path_;
        std::string description_;
        std::vector<ncs::parameter> parameters_;
        std::function<void()> function_;
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