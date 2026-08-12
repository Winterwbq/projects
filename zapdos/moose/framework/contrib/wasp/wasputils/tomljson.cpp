/*
 * File:   tomljson.cpp
 */

#include <cstdlib>
#include <iostream>
#include <fstream>
#include "waspcore/TokenPool.h"
#include "waspcore/TreeNodePool.h"
#include "wasptoml/TOMLInterpreter.h"
#include "wasptoml/TOMLNodeView.h"
#include "waspcore/version.h"
#include "wasphive/HIVE.h"
using namespace wasp;

typedef DefaultTOMLInterpreter TOMLInterp;

int main(int argc, char** argv)
{
    if (argc == 2 &&
        (std::string(argv[1]) == "-v" || std::string(argv[1]) == "--version"))
    {
        std::cout << wasp_version_info::name << " "
                  << wasp_version_info::full_version << std::endl;
        return 0;
    }

    if (argc != 2)
    {
        std::cerr
            << "Workbench Analysis Sequence Processor - TOML to JSON Converter"
            << std::endl
            << " Usage: " << argv[0]
            << " path/to/TOML/formatted/input"
            << std::endl
            << " Usage: " << argv[0] << " --version\t(print version info)"
            << std::endl;
        return 1;
    }

    std::stringstream errors;

    // parse input
    TOMLInterp input_interp(errors);
    bool      parsed_input = input_interp.parseFile(argv[1]);
    if (!parsed_input)
    {
        std::cerr << "Failed to process input file '" << argv[1] << "'"
                  << std::endl;
        std::cerr << errors.str() << std::endl;
        return 1;
    }
    bool generated = input_interp.generate_object();
    if (!generated)
    {
        std::cout << input_interp.error_diagnostics() << std::endl;
    }
    if (input_interp.object() != nullptr)
    {
        input_interp.object()->pack_json(std::cout); 
        std::cout << std::endl;
    }

    return generated ? 0 : 1;
}
