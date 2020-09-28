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
    
    ncs::init<ncs::command> version{ this, "version", [this]{ cli.help(); }, "Display compiler version" };

    //ncs::init<ncs::command> version{ this, "", []{}, "Display compiler version" };

    struct : node
    {
        using node::node;
        ncs::init<ncs::command> add{ this, "add"
        , [this]{ cli.compiler.process("test"); }
        , "Add a new project"
            , ncs::parameter{ "name", "Project name" }
            , ncs::parameter{ "vcs", "Initialize VCS", "git"s } };
    } project{ this, "project" };
};

int main(int argc, const char* argv[])
{
    ::compiler compiler;
    ::ngl_cli ngl_cli{ "nc", compiler };

    nc_commands nc{ ngl_cli, ngl_cli.module_name() };


    ngl_cli.help();

    ngl_cli.process(argc, argv);

    return 0;
}
