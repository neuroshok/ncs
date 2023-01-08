#include <ncs/module/project/generator.hpp>

#include <ncs/module/project/project.hpp>

#include <ncs/core.hpp>
#include <ncs/parameters.hpp>
#include <ncs/utility.hpp>

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
                for (const auto& [parameter, value] : variables_)
                {
                    str_replace(command, parameter, value);
                }
                commands_.emplace_back(command);
            }
            core_.log("Loading {} commands", commands_.size());
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
                variables_.emplace_back(meta_prefix_ + "." + parameter, value);
            }
            core_.log("Loading {} variables", variables_.size());
        }
        else core_.log("No variables found at {} ", variables_path);

        variables_.emplace_back("$ncs.project.name", project_name_);
    }

    void generator::process(const ncs::parameters& parameters)
    {
        try
        {
            std::vector<fs::path> pending_rename;

            project_name_ = parameters.value("name");
            std::string source_str = parameters.value("tpl");
            std::string input_license = parameters.value("license");

            // arkena00/nps.git

            source_origin_ = fs::path{ source_str };
            if (!std::filesystem::exists(source_origin_)) return core_.log("Source directory not found: {}", source_origin_.generic_string());
            
            load_variables();
            load_commands();

            auto target_origin = fs::current_path() / fs::path{ project_name_ };
            std::string target_str = target_origin.string();

            core_.log("Make target directory {}", target_str);
            fs::create_directory(target_origin);

            core_.log("Generate template from {}", source_str);

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
                    if (is_meta) pending_rename.emplace_back(target_origin / entry_path);
                    fs::create_directory(target_origin / entry_path);
                    continue;
                }
                else if (!source_entry.is_regular_file()) continue; // ignore . ..

                auto file_content = process_file(source_origin_, source_entry.path());

                // output
                fs::path output_path;
                if (is_meta) output_path = target_origin / fs::relative(source_entry.path().parent_path(), source_origin_) / evaluate_name(file_name);
                else output_path = target_origin / entry_path;

                std::ofstream ofs{ output_path };
                if (ofs.is_open())
                {
                    ofs.write(file_content.data(), file_content.size());
                    ofs.close();
                }
                else core_.log("Error opening file {}", (target_origin / entry_path).generic_string());
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
                core_.log("> {}", command);
                //system(command.c_str());
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
                auto input_path = file_content;
                str_replace(input_path, ".", "/");
                auto reference_path = source_origin_ / input_path /*/ input_license*/;

                std::ifstream cfs{ reference_path };
                if (cfs.is_open())
                {
                    file_content = { std::istreambuf_iterator<char>(cfs), std::istreambuf_iterator<char>() };
                }
                else core_.log("Error opening file {}", reference_path.generic_string());
            }

            for (const auto& [parameter, value] : variables_)
            {
                str_replace(file_content, parameter, value);
            }

            return file_content;
        }
    }

    std::string generator::evaluate_name(const std::string& name)
    {
        std::string output = name;
        for (const auto& [variable, value] : variables_)
        {
            if (name.starts_with(variable))
            {
                str_replace(output, variable, value);
                return value;
            }
        }

        return name;
    }
} // ncs