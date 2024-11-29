#include <iostream>
#include <fstream>

#include <fmt/format.h>

#include <boost/program_options.hpp>
namespace po = boost::program_options;

#define GEOMETRY_FILE_PATH "geom.in"

#define QCHEM_INPUT_FILE  "f.inp"
#define QCHEM_OUTPUT_FILE "f.out"

#include "IOManagers/InputParser.hpp"

int main(int argc, char *argv[])
{
    // Create Command Line Options
    po::options_description visibleOptionsDescription("Invocation: <program> --help <input-file>"); // TODO: Better Description Needed
    visibleOptionsDescription.add_options()
        ("help", "Produce a Help Message")
        ("version,v", "Display the Program's Version");
    
    po::options_description hiddenOptionsDescription;
    hiddenOptionsDescription.add_options()
        ("input-file", po::value<std::string>(), "Tested Input File");

    po::options_description optionsDescription;
    optionsDescription.add(visibleOptionsDescription).add(hiddenOptionsDescription);

    // Create Positional Options
    po::positional_options_description positionalOptionsDescription;
    positionalOptionsDescription.add("input-file", 1);

    // Create Variable Map
    po::variables_map variablesMap;
    po::store(po::command_line_parser(argc, argv).options(optionsDescription).positional(positionalOptionsDescription).run(), variablesMap);
    po::notify(variablesMap);

    // Print Help Message if Needed
    if (variablesMap.count("help")) {
        std::cout << visibleOptionsDescription << std::endl;
        return 0;
    }

    // Initialise File Parser
    InputParser inputParser;

    // Create the Q-Chem Input File
    std::ofstream qchemFile(fmt::format("{0}/{1}/{2}", RESULTS_PATH, ))

    return 0;
}