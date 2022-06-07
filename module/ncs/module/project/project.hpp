#ifndef MODULE_PROJECT_PROJECT_HPP_NCS
#define MODULE_PROJECT_PROJECT_HPP_NCS

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
        {}

        void make(const ncs::parameters& input)
        {
            mod::generator generator{ core_, "$ncs" };
            generator.process(input);
        }

    private:
        ncs::core& core_;
    };
} // ncs

#endif // MODULE_PROJECT_PROJECT_HPP_NCS