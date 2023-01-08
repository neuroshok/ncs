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

struct commands : public ncs::commands<::commands>
{
    commands(std::string module_name, ncs::modules::project& c) : ncs::commands<::commands>(std::move(module_name)), project{ c } {}

    ncs::modules::project& project;
};

// clang-format off
ncs_root(commands_root)
    ncs_command(ncs_module)

    ncs_command_(ncs_module, [this]{ cmd.help(); }, "")

    ncsi::command help{ this, "help", [this]{  cmd.help(); }, "Display this help" };
    ncsi::command version{ this, "version", []{ std::cout << "0.2.1"; }, "Display ncs version" };

/*
    ncs_command(project)
        ncs_required(tpl, std::string, "Location to the project template")
        ncs_parameter(name, std::string, "project", "Project name")
        ncs_parameter(license, std::string, "mit", "License")
    ncs_command_(project,  [this](auto&& input){  }, "")*/
ncs_root_()
// clang-format on

int main(int argc, const char* argv[])
{
    /*
    using namespace ncs::modules;
    ncs::modules<ngl::compiler, project> modules;
*/

    ncs::core core{ argc, argv };
    core.add_module<ncs::modules::project>();
    core.process();

    return 0;
}