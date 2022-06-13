#include <ncs/ncs.hpp>

#include <ncs/core.hpp>
#include <ncs/module/project/project.hpp>

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

struct ngl_cli : public ncs::basic_cli<::ngl_cli>
{
    ngl_cli(std::string module_name, ncs::modules::project& c) : ncs::basic_cli<::ngl_cli>(std::move(module_name)), project{ c } {}

    ncs::modules::project& project;
};

// clang-format off
ncs_root(nc_commands, ::ngl_cli)
    ncs_command(ncs_module)

    ncs_command_(ncs_module, [this]{ cli.help(); }, "")

    ncsi::command help{ this, "help", [this]{  cli.help(); }, "Display this help" };
    ncsi::command version{ this, "version", []{ std::cout << "0.2.1"; }, "Display ncs version" };


    ncs_command(project)
        ncs_required(tpl, std::string, "Location to the project template")
        ncs_parameter(name, std::string, "project", "Project name")
        ncs_parameter(license, std::string, "mit", "License")
    ncs_command_(project,  [this](auto&& input){ cli.project.make(input); }, "")
ncs_root_()
// clang-format on

int main(int argc, const char* argv[])
{
    /*
    ncs::cli cli{ argv[0] };
    nc_commands nc{ cli };
    cli.process(argc, argv);*/

    /*
    using namespace ncs::modules;
    ncs::modules<ngl::compiler, project> modules;

    ncs::core core{ argc, argv, modules };*/

    ncs::core core{ argc, argv };
    core.process();
    // core.process("nxi", "new_page search_term -s:google");
    // core.add_command();

    ncs::modules::project project{ core };
    ::ngl_cli ngl_cli{ argv[0], project };

    nc_commands nc{ ngl_cli };
    ngl_cli.process(argc, argv);



    //ngl_cli.process("ncs project add -name:test -vcs");



    return 0;
}