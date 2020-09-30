#include <ncs/cli.hpp>
#include <ncs/command_initializer.hpp>

using namespace std::string_literals;

struct compiler
{
    void process(std::string filename, int j) { std::cout << "\ncompile file " << filename; }
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
    
    ncsi::command help{ this, "help", [this]{ cli.help(); }, "Display this help" };
    //ncs::init<ncs::command> version{ this, "version", [this]{ std::cout << "0.2.1"; }, "Display compiler version" };

/*
    struct:node{using node::node;
        ncs::init<ncs::command> add{ this, "add"
        , [this]{ std::cout << "add entity"; }
        , "Add a new entity"
            , ncs::parameter{ "name", "Entity name" }
            , ncs::parameter{ "mail", "", ""s } };
    } entity{ this, "entity" };*/

    ncs_node(project)
        ncs_command(add)
            ncs_parameter(std::string, name, "Project name")
            ncs_parameter(std::string, vcs, "Init VCS", "git"s)
        ncs_command_(add, [this]{ cli.compiler.process("test", 0); }, "Add a new project")
    ncs_node_(project)
};


int main(int argc, const char* argv[])
{
    ::compiler compiler;
    ::ngl_cli ngl_cli{ "ncs", compiler };

    nc_commands nc{ ngl_cli, ngl_cli.module_name() };

    //ngl_cli.help();

    ngl_cli.process(argc, argv);

    return 0;
}
