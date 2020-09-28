#include <ncs/cli.hpp>

struct nc_commands :ncs::node
{
    using ncs::node::node;
    
    ncs::init<ncs::command> version{ this, "version", []{}, "Display compiler version" };

    struct :ncs::node
    {
        using ncs::node::node;
        ncs::init<ncs::command> add{ this, "add", []{}, "Add a new project" };
    } project{ this, "project" };

};

int main(int argc, const char* argv[])
{
    ncs::cli cli{ "nc" };

    nc_commands nc{ cli, cli.module_name() };

/*
    cli.add("version", [](){ std::cout << "nc version 0.1"; }, ""
    , ncs::parameter{ "test", "Test filename", true }
    , ncs::parameter{ "zeta" }
    );

 
    cli.add(ncs::path("ncs", "project", "add")
    , []{}
    , "Add a new project"
    , ncs::parameter{ "name", "Project filename" }
    , ncs::parameter{ "vcs", "Enable vcs", std::string("git") });
*/

    cli.help();

    cli.process(argc, argv);

    return 0;
}
