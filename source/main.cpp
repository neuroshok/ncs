#include <ncs/ncs.hpp>

struct compiler
{
    template<class T>
    void make_project(const ncs::named_parameters<T>& input)
    {
        std::cout << "\nout:\n";
        std::cout << input[input.param.name] << std::endl;
        if (input.has(input.param.vcs)) std::cout << input[input.param.vcs] << std::endl;
        std::cout << input[input.param.verbose] << std::endl;
        std::cout << input[input.param.j] << std::endl;
    }
};

struct ngl_cli : public ncs::cli<::ngl_cli>
{
    ngl_cli(std::string name, ::compiler& c) : ncs::cli<::ngl_cli>(std::move(name)), compiler{ c } {}

    ::compiler& compiler;
};

ncs_root(nc_commands)
    ncsi::command help{ this, "help", [this]{  cli.help(); }, "Display this help" };
    ncsi::command version{ this, "version", []{ std::cout << "0.2.1"; }, "Display compiler version" };

    ncs_node(project)
        ncs_command(add)
            ncs_required(name, std::string, "Project name")
            ncs_optional(vcs, std::string, "git", "Initialize VCS")
            ncs_flag(verbose)
            ncs_parameter(j, int, 4, "Threads to build")
        ncs_command_(add, [this](auto&& input){ /*cli.compiler.make_project(ncs::named_parameters(add, input));*/ }, "Add a new project")
    ncs_node_(project)
ncs_root_()

int main(int argc, const char* argv[])
{
    ::compiler compiler;
    ::ngl_cli ngl_cli{ "ncs", compiler };

    //nc_commands nc{ "test" };
    nc_commands nc;
    nc.cli.process(argc, argv);

    //ngl_cli.process(argc, argv);

    return 0;
}
