#include <ncs/module/project/generator.hpp>

#include <ncs/module/project/project.hpp>

#include <ncs/core.hpp>
#include <ncs/parameters.hpp>
#include <ncs/utility.hpp>

#include "tiny-process-library/process.hpp"

#include <filesystem>
#include <fstream>

namespace ncs::internal::modules::project
{
    namespace fs = std::filesystem;

    generator::generator(ncs::modules::project& project, std::string meta_prefix_)
        : core_{ project.core() }
        , meta_prefix_{ std::move(meta_prefix_) }
    {}

    void generator::load_commands()
    {
        const auto commands_path = source_origin_.generic_string() + "/" + meta_prefix_ + "/commands.txt";
        std::ifstream ifs{ commands_path };

        if (ifs.is_open())
        {
            std::string command;
            while (std::getline(ifs, command))
            {
                if (command.empty() || !command.empty() && command[0] == '#') continue;
                for (const auto& [parameter, value] : variables_)
                {
                    str_replace(command, meta_prefix_ + "." + parameter, value);
                }
                commands_.emplace_back(command);
            }
            //core_.log("Loading {} commands", commands_.size());
        }
        else core_.log("No commands found at {} ", commands_path);
    }

    void generator::load_variables()
    {
        const auto variables_path = source_origin_.generic_string() + "/" + meta_prefix_ + "/variables.txt";
        std::ifstream ifs{ variables_path };

        if (ifs.is_open())
        {
            std::string parameter;
            std::string value;
            while (ifs >> parameter >> value)
            {
                variables_.emplace_back(parameter, value);
            }

            core_.log("Loading {} variables...", variables_.size());
            for (auto& [variable_name, variable_value] : variables_)
            {
                std::string user_variable;
                core_.message("> {} (default: {}): ", variable_name, variable_value);
                std::getline(std::cin, user_variable);
                if (!user_variable.empty()) variable_value = user_variable;
            }
        }
        else core_.log("No variables found at {} ", variables_path);

        variables_.emplace_back("project.name", project_name_);
    }

    void generator::process(const ncs::parameters& parameters)
    {
        try
        {
            dynamic_inputs_ = parameters;
            std::vector<fs::path> pending_rename;

            project_name_ = parameters.value("name");
            std::string source_str = parameters.value("tpl");

            target_origin_ = fs::current_path() / fs::path{ project_name_ };
            std::string target_str = target_origin_.string();

            // tpl source
            if (source_str.starts_with("gh:"))
            {
                auto slash_pos = source_str.find('/');
                std::string user = source_str.substr(3, slash_pos - 3);
                std::string template_name = source_str.substr(1 + slash_pos);
                std::string repo = "nps." + template_name;

                core_.log("Loading template from github: {}/{}", user, repo);
                run("git clone https://github.com/" + user + "/" + repo + ".git");
                source_origin_ = fs::path{ fs::current_path() / repo };
                clean_source_ = true;
            }
            else
            {
                source_origin_ = fs::path{ source_str };
                if (!std::filesystem::exists(source_origin_)) return core_.log("Source directory not found: {}", source_origin_.generic_string());
                core_.log("Loading template from path: {}", source_str);
            }

            load_variables();
            load_commands();

            core_.log("Make target directory {}", target_str);
            fs::create_directory(target_origin_);

            for (const auto& source_entry : fs::recursive_directory_iterator(source_origin_))
            {
                auto entry_path = fs::relative(source_entry.path(), source_origin_);
                if (entry_path.generic_string().starts_with(meta_prefix_ + "/") || entry_path == meta_prefix_) continue;

                auto file_name = source_entry.path().filename().generic_string();
                bool is_meta = file_name.starts_with(meta_prefix_);

                //core_.log("Processing entry {} dir: {} meta: {}", file_name, source_entry.is_directory(), is_meta);

                if (source_entry.is_directory())
                {
                    // if the directory path has meta, keep source path and rename it later (/$meta/folder/)
                    if (is_meta) pending_rename.emplace_back(target_origin_ / entry_path);
                    fs::create_directory(target_origin_ / entry_path);
                    continue;
                }
                else if (!source_entry.is_regular_file() || file_name == ".git") continue; // ignore . ..

                auto file_content = process_file(source_origin_, source_entry.path());

                // output
                fs::path output_path;
                if (is_meta) output_path = target_origin_ / fs::relative(source_entry.path().parent_path(), source_origin_) / evaluate_name(file_name);
                else output_path = target_origin_ / entry_path;

                std::ofstream ofs{ output_path };
                if (ofs.is_open())
                {
                    ofs.write(file_content.data(), file_content.size());
                    ofs.close();
                }
                else core_.log("Error opening output file {}", (target_origin_ / entry_path).generic_string());
            }

            // rename pending directories
            for (const auto& path : pending_rename)
            {
                auto pending_name = path.filename().generic_string();
                fs::rename(path, path.parent_path() / evaluate_name(pending_name));
            }

            // run commands
            core_.log("Running {} commands...", commands_.size());
            for (const auto& command : commands_)
            {
                run(command, target_origin_);
            }

            // cleaning repo
            if (clean_source_)
            {
                core_.log("Clean source");
                fs::remove_all(source_origin_);
            }

            core_.log("Generation complete");
        }
        catch (const std::exception& e)
        {
            std::cout << "Exception: " << e.what();
        }
    }

    std::string generator::process_file(const std::filesystem::path& source_origin_, const std::filesystem::path& file)
    {
        std::ifstream ifs{ file };

        if (!ifs.is_open()) core_.log("Error opening file {}", file.generic_string());
        else
        {
            std::string file_content{ std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>() };

            // process file containing $ncs.input
            if (file_content.starts_with(meta_prefix_ + ".input"))
            {
                file_content = get_input_data(file_content);
            }

            for (const auto& [parameter, value] : variables_)
            {
                str_replace(file_content, meta_prefix_ + "." + parameter, value);
            }

            return file_content;
        }
    }

    std::string generator::evaluate_name(const std::string& name)
    {
        std::string output = name;
        int i = 0;
        for (const auto& [variable, value] : variables_)
        {
            if (name.starts_with(meta_prefix_ + "." + variable))
            {
                str_replace(output, meta_prefix_ + "." + variable, value);
                return output;
            }
            ++i;
        }
        if (i == 0) core_.log("Variable not found for '{}'", name);

        return name;
    }

    std::string generator::get_input_data(const std::string& input_path)
    {
        std::string input = input_path;

        std::string file_content;
        fs::path reference_path;
        for (const auto& [name, parameter] : dynamic_inputs_)
        {
            if (meta_prefix_ + "." + name == input)
            {
                str_replace(input, ".", "/");

                reference_path = source_origin_ / input / std::get<std::string>(parameter.value);
                if (!fs::exists(reference_path))
                {
                    core_.log("Dynamic input file not found: {}", reference_path.generic_string());
                    return "";
                }
                break;
            }
        }

        std::ifstream cfs{ reference_path };
        if (cfs.is_open())
        {
            file_content = { std::istreambuf_iterator<char>(cfs), std::istreambuf_iterator<char>() };
        }

        return file_content;
    }

    void generator::run(const std::string& command, const fs::path& execution_path)
    {
        core_.log("> {}", command);

        std::vector<std::string> process_args;
        for (auto line : std::views::split(command, " "))
        {
            process_args.emplace_back(line.begin(), line.end());
        }

        for (const auto& arg : process_args)
        TinyProcessLib::Process process(
            process_args,
            execution_path.generic_string(),
            [&command](const char* bytes, size_t n) {
                //std::cout << "\n" << std::string{ bytes, n } << std::endl;
            },
            [&command](const char* bytes, size_t n) {
                //std::cout << "\n" << std::string{ bytes, n } << std::endl;
            });

    }
} // ncs