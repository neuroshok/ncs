#ifndef MODULE_PROJECT_PROJECT_HPP_NCS
#define MODULE_PROJECT_PROJECT_HPP_NCS

#include <filesystem>
#include <format>
#include <fstream>
#include <ncs/module/project/generator.hpp>

/*
// path relative to entity root
ncs project make -path:ark/cpp -license:mit -name:ncs -vcs:git -ncs.default<path license vcs> // default parameters
ncs project make -name:test // use default parameters, name required

//
ncs context project xmake
ncs make -name:test
ncs config --qt:d:/qt // xmake command
ncs build // cmake build

//
ncs project make -path:ark/cpp -license:mit -name:ncs -vcs:git
ncs edge @ark https://www.github.com/arkena00
ncs edge @jo https://www.github.com/jo
ncs project make @jo:cpp

// ncs status or ncs info

 ncs project make -tpl:ark/cpp -name:test
*/

#include <ncs/parameter.hpp>

namespace ncs::modules
{
    namespace mod = ncs::internal::modules::project;

    class project
    {
    public:
        explicit project(ncs::core& core)
            : core_{ core }
            , generator_{}
        {}

        template<class T>
        void make(const ncs::named_parameters<T>& input)
        {
            std::string meta_prefix = "$ncs";
            std::string var_project_name = "project.name";
            generator_.source = input[input.param.tpl];

            try
            {
                namespace fs = std::filesystem;
                std::string project_name = input[input.param.name];

                std::string source_str = input[input.param.tpl];
                auto source_origin = fs::path{ source_str };
                auto target_origin = fs::current_path() / fs::path{ project_name };
                std::string target_str = target_origin.string();

                core_.log("Make target directory {}", target_str);
                fs::create_directory(target_origin);

                core_.log("Generate template from {}", source_str);

                for (const auto& source_entry : fs::recursive_directory_iterator(source_origin))
                {
                    auto entry_path = fs::relative(source_entry.path(), source_origin);
                    if (entry_path.generic_string().starts_with(meta_prefix + "/") || entry_path == meta_prefix) continue;

                    auto file_name = source_entry.path().filename().generic_string();

                    bool is_meta = file_name.starts_with(meta_prefix);

                    // core_.log("process {}", entry_path.generic_string());

                    if (!is_meta && source_entry.is_directory())
                    {
                        fs::create_directory(target_origin / entry_path);
                        continue;
                    }

                    if (is_meta)
                    {
                        // todo parse variable ncs project name | ncs input parameter
                        if (file_name.substr(meta_prefix.size() + 1, var_project_name.size()) == var_project_name)
                        {
                            auto generated_file_name = project_name + file_name.substr(meta_prefix.size() + 1 + var_project_name.size());
                            auto generated_target =
                            target_origin / fs::relative(source_entry.path().parent_path(), source_origin) / generated_file_name;
                            // core_.log("copy {} => {}", source_entry.path().generic_string(), generated_target.generic_string());
                            fs::copy(source_entry.path(), generated_target, fs::copy_options::update_existing);
                        }
                        else core_.log("Unknown variable while processing file '{}'", file_name);
                    }
                    else
                    {
                        // core_.log("copy {} => {}", source_entry.path().generic_string(), (target_origin / entry_path).generic_string());
                        fs::copy(source_entry, target_origin / entry_path, fs::copy_options::update_existing);
                    }

                    if (!source_entry.is_regular_file()) continue;


                    std::ifstream ifs{ source_entry.path() };

                    if (!ifs.is_open()) core_.log("Error opening file {}", file_name);
                    else
                    {
                        std::string file_content{ std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>() };
                        // process_content(file_content);
                        if (file_content.starts_with(meta_prefix + ".input"))
                        {
                            auto input_path = file_content;
                            str_replace(input_path, ".", "/");
                            auto reference_path = source_origin / input_path / input[input.param.license];

                            std::ifstream cfs{ reference_path };
                            if (cfs.is_open())
                            {
                                file_content = { std::istreambuf_iterator<char>(cfs), std::istreambuf_iterator<char>() };
                            }
                            else core_.log("Error opening file {}", reference_path.generic_string());
                        }
                        str_replace(file_content, "$ncs.project.name", project_name);
                        str_replace(file_content, "$ncs.entity.name", "Arkena");

                        std::ofstream ofs{ target_origin / entry_path };
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

        /*
        void process_content(std::string& file_content, const ncs::named_parameters<T>& input) {
            str_replace(file_content, "$ncs.project.name", inputs_[]) }*/

        void process() // vector<option>
        {
            std::string origin = "d:/tmp/ncs/";
        }

    private:
        ncs::core& core_;

        mod::generator generator_;
    };
} // ncs

#endif // MODULE_PROJECT_PROJECT_HPP_NCS