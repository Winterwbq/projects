#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <map>
#include "waspcore/version.h"
#include "waspcore/wasp_bug.h"
#include "waspcore/Object.h"
#include "waspjson/JSONObjectParser.hpp"
#include "waspmcnpi/MCNPInterpreter.h"
#include "waspmcnpi/Model.h"

using namespace wasp;

void dump_errors(DataArray& da_error)
{
    if(!da_error.empty())
    {
        da_error.format_json(std::cout);
    }
}

int main(int argc, char* argv[])
{
    if (argc == 2 &&
        (std::string(argv[1]) == "-v" || std::string(argv[1]) == "--version"))
    {
        std::cout << wasp_version_info::name << " "
                  << wasp_version_info::full_version << std::endl;
        return 0;
    }

    if (argc == 2)
    {
        std::cout << "Workbench Analysis Sequence Processor (WASP)" << std::endl
                  << argv[0] << " : MCNP to JSON (materials) Converter"
                  << std::endl;
        std::cout << " Usage : " << argv[0] << " path/to/MCNP/formatted/input path/to/zaid-molar-masses.json" << std::endl
                  << " zaid-molar-masses.json format requirement is {\"zaid\": mass, ...}. " << std::endl 
                  << " For example {\"1001\": 1.007830023765564,...} " << std::endl;
        std::cout << " Usage : " << argv[0]
                  << " --version\t(print version info)" << std::endl;
        return 1;
    }


    DataArray da_error;

    std::stringstream        errors;
    DataObject::SP zaid_molar_masses_json; 
    std::ifstream masses_stream(argv[2]);
    if (!masses_stream.good())
    {
        std::cerr << "Unable to read " << argv[2] << "!" << std::endl;
        return 1;
    }

    // Attempt to interpreter the molar masses
    JSONObjectParser generator(zaid_molar_masses_json, masses_stream, errors, nullptr);

    if (generator.parse() != 0)
    {
        da_error.push_back(errors.str());
        dump_errors(da_error); return -1;
    }

    // Convert json object to map<int, double>
    std::map<int, double> molar_masses; 
    for (auto itr = zaid_molar_masses_json->begin(); itr != zaid_molar_masses_json->end(); itr++)
    {
        molar_masses[std::stoi(itr->first)] = itr->second.to_double();
    }

    // Attempt to parse the document
    DefaultMCNPInterpreter interpreter(errors);
    interpreter.setStreamName(argv[1]);
    std::ifstream mcnpdata(argv[1]);
    bool parsed = interpreter.parse(mcnpdata);
    if (!parsed)
    {
        da_error.push_back(errors.str());
        dump_errors(da_error); return -1;
    }

    // Next try to construct the MCNP model
    mcnpi::Model model;
    model.set_zaid_relative_atomic_mass_map(&molar_masses);
    bool success = model.build(interpreter.root(), errors);
    if (!success)
    {
        da_error.push_back(errors.str());
        dump_errors(da_error); return -1;
    }
    // Output the materials in JSON format
    model.describe_materials_json(std::cout);       
    
    return 0;
}
