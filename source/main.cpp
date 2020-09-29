#include <ncs/cli.hpp>

using namespace std::string_literals;

struct compiler
{
    void process(std::string filename) { std::cout << "\ncompile file " << filename; }
};

struct ngl_cli : public ncs::cli<::ngl_cli>
{
    ngl_cli(std::string name, ::compiler& c) : ncs::cli<::ngl_cli>(std::move(name)), compiler{c} {}
    ::compiler& compiler;
};

using node = ncs::node<::ngl_cli>;
struct nc_commands : node
{
    using node::node;
    
    ncs::init<ncs::command> help{ this, "help", [this]{ cli.help(); }, "Display this help" };
    ncs::init<ncs::command> version{ this, "version", [this]{ std::cout << "0.2.1"; }, "Display compiler version" };

    struct : node
    {
        using node::node;
        ncs::init<ncs::command> add{ this, "add"
        , [this]{ std::cout << "add entity"; }
        , "Add a new entity"
            , ncs::parameter{ "name", "Entity name" }
            , ncs::parameter{ "mail", "", ""s } };
    } entity{ this, "entity" };

    struct : node
    {
        using node::node;
        ncs::init<ncs::command> add{ this, "add"
        , [this]{ cli.compiler.process("test", param.); /* param[""] */ }
        , "Add a new project"
            , ncs::parameter{ "name", "Project name" }
            , ncs::parameter{ "vcs", "Initialize VCS", "git"s } };
    } project{ this, "project" };
};


int main(int argc, const char* argv[])
{
    ::compiler compiler;
    ::ngl_cli ngl_cli{ "ncs", compiler };

    nc_commands nc{ ngl_cli, ngl_cli.module_name() };
    nc.project.add.set_function([&compiler](auto&& param)
    { compiler.add_project(param[nc.project.add.name]); });

    //ngl_cli.help();

    ngl_cli.process(argc, argv);

    return 0;
}
