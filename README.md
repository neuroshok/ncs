# Neuroshok Command System

## Usage
### Basic
```cpp
ncs_root(my_commands) // my_commands : generated structure
    ncsi::command help{ this, "help", [this]{  cli.help(); }, "Display this help" };
    ncsi::command version{ this, "version", []{ std::cout << "0.2.1"; }, "Display program version" };
ncs_root_()

int main(int argc, const char* argv[])
{
    ncs::cli cli{ argv[0] };
    nc_commands nc{ cli };
    cli.process(argc, argv);

    return 0;
}
```

Assuming the program name is "ncs", you can write
- `ncs help`
- `ncs version`

### Custom cli
```cpp
struct compiler
{
    template<class T>
    void make_project(const ncs::named_parameters<T>& input)
    {
        std::cout << input[input.param.name] << std::endl;
        if (input.has(input.param.vcs)) std::cout << input[input.param.vcs] << std::endl;
        std::cout << input[input.param.verbose] << std::endl;
        std::cout << input[input.param.j] << std::endl;
    }
};

struct ngl_cli : public ncs::basic_cli<::ngl_cli>
{
    ngl_cli(std::string module_name, ::compiler& c) : ncs::basic_cli<::ngl_cli>(std::move(module_name)), compiler{ c } {}
    ::compiler& compiler;
};

ncs_root(nc_commands, ::ngl_cli)
    ncsi::command help{ this, "help", [this]{  cli.help(); }, "Display this help" };
    ncsi::command version{ this, "version", []{ std::cout << "0.2.1"; }, "Display compiler version" };

    // description is optional
    // bool parameters are interpreted as flags
    // ncs_required, ncs_optional, ncs_flag are aliases for ncs_parameter
    ncs_node(project)
        ncs_command(add)
            ncs_required(name, std::string, "Project name")
            ncs_optional(vcs, std::string, "git", "Initialize VCS")
            ncs_flag(verbose)
            ncs_parameter(j, int, 4, "Threads to build")
        ncs_command_(add, [this](auto&& input){ cli.compiler.make_project(ncs::named_parameters(add, input)); }, "Add a new project")
    ncs_node_(project)
ncs_root_()

int main(int argc, const char* argv[])
{
    ::compiler compiler;
    ::ngl_cli ngl_cli{ argv[0], compiler };
    nc_commands nc{ ngl_cli };
    ngl_cli.process(argc, argv);

    return 0;
}
```
Assuming the program name is "ncs", you can write
- `ncs project add -name:my_project -vcs -j:4 -verbose`

## Input behaviour
Using those parameters definition and a program named "nc"
```
    ncs_node(project)
        ncs_command(add)
            ncs_required(name, std::string, "Project name")
            ncs_optional(vcs, std::string, "git", "Initialize VCS")
            ncs_flag(verbose)
            ncs_parameter(j, int, 4, "Threads to build")
        ncs_command_(add, [this](auto&& input){ cli.compiler.make_project(ncs::named_parameters(add, input)); }, "Add a new project")
    ncs_node_(project)
```

- *todo* `nc project`
Error: display available commands

- `nc project add`
Error: missing parameter <name>

- `nc project add -name`
Error: missing parameter <name> value

- `nc project add -name:my_project`
Ok: vcs does not exist in input but default value is accessible (use input.has(input.param.vcs) if you need to check its existence)

- `nc project add -name:my_project -vcs:nvs`
Ok, use explicit value

- Full command: `nc project add -name:my_project -vcs -verbose -j:4`