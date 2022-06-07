#include <ncs/module/project/generator.hpp>

#include <ncs/core.hpp>
#include <ncs/parameters.hpp>
#include <ncs/utility.hpp>

#include <filesystem>
#include <fstream>

namespace ncs::internal::modules::project
{
    namespace fs = std::filesystem;

    generator::generator(ncs::core& core, std::string meta_prefix_)
        : core_{ core }
        , meta_prefix_{ std::move(meta_prefix_) }
    {}

    void generator::process(const ncs::parameters& parameters)
    {

        try
        {
            project_name_ = parameters.value("name");
            std::string source_str = parameters.value("tpl");
            std::string input_license = parameters.value("license");

            // arkena00/nps.git

            auto source_origin = fs::path{ source_str };
            auto target_origin = fs::current_path() / fs::path{ project_name_ };
            std::string target_str = target_origin.string();

            core_.log("Make target directory {}", target_str);
            fs::create_directory(target_origin);

            core_.log("Generate template from {}", source_str);

            for (const auto& source_entry : fs::recursive_directory_iterator(source_origin))
            {
                auto entry_path = fs::relative(source_entry.path(), source_origin);
                if (entry_path.generic_string().starts_with(meta_prefix_ + "/") || entry_path == meta_prefix_) continue;

                auto file_name = source_entry.path().filename().generic_string();

                bool is_meta = file_name.starts_with(meta_prefix_);

                if (source_entry.is_directory())
                {
                    if (!is_meta) fs::create_directory(target_origin / entry_path);
                    else
                        fs::create_directory(target_origin / fs::relative(source_entry.path().parent_path(), source_origin) /
                                             evaluate_name(file_name));
                    continue;
                }
                else if (!source_entry.is_regular_file()) continue;

                // process file
                std::ifstream ifs{ source_entry.path() };

                if (!ifs.is_open()) core_.log("Error opening file {}", file_name);
                else
                {
                    std::string file_content{ std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>() };
                    if (file_content.starts_with(meta_prefix_ + ".input"))
                    {
                        auto input_path = file_content;
                        str_replace(input_path, ".", "/");
                        auto reference_path = source_origin / input_path / input_license;

                        std::ifstream cfs{ reference_path };
                        if (cfs.is_open())
                        {
                            file_content = { std::istreambuf_iterator<char>(cfs), std::istreambuf_iterator<char>() };
                        }
                        else core_.log("Error opening file {}", reference_path.generic_string());
                    }
                    str_replace(file_content, "$ncs.project.name", project_name_);
                    str_replace(file_content, "$ncs.entity.name", "Arkena");

                    // output
                    fs::path output_path;
                    if (is_meta)
                        output_path = target_origin / fs::relative(source_entry.path().parent_path(), source_origin) / evaluate_name(file_name);
                    else output_path = target_origin / entry_path;

                    std::ofstream ofs{ output_path };
                    if (ofs.is_open())
                    {
                        ofs.write(file_content.data(), file_content.size());
                        ofs.close();
                    }
                    else core_.log("Error opening file {}", (target_origin / entry_path).generic_string());
                }
            }

            core_.log("Generation complete");
        }
        catch (const std::exception& e)
        {
            std::cout << e.what();
        }
    }

    std::string generator::evaluate_name(const std::string& name)
    {
        if (name.substr(meta_prefix_.size() + 1, var_project_name.size()) == var_project_name)
        {
            return project_name_ + name.substr(meta_prefix_.size() + 1 + var_project_name.size());
        }
        else core_.log("Unknown variable name '{}'", name);

        return name;
    }
} // ncs