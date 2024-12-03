#include <iostream>
#include <fstream>

#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include "IOManagers/QChemConfig.hpp"

#define QCHEM_INPUT_FILE  "f.inp"
#define QCHEM_OUTPUT_FILE "f.out"

/// @brief 
/// @param a_geometryInput 
/// @param a_numCPUs 
/// @param a_spinFlip 
/// @param a_scfAlgorithm 
/// @param a_theory 
/// @param a_guess 
/// @return Whether the Job was Submitted Successfully
bool SubmitQChemJob(QChemConfig a_qchemConfig)
{
    return false;
}

int main(int argc, char *argv[])
{
    // Create Command Line Options
    po::options_description genericOptions("Generic Options");
    genericOptions.add_options()
        ("help", "Produce a Help Message")
        ("version,v", "Display the Program's Version");

    po::options_description configOptions("Configuration Options");
    genericOptions.add_options()
        ("geometry-input", po::value<std::string>(), "File Containing the Input Geometry")
        ("num-cpus,nc", po::value<int>(), "Number of CPUs used during the Computation")
        ("spin-flip", po::value<bool>(), "")
        ("scf-algorith", po::value<std::string>(), "")
        ("theory", po::value<int>(), "")
        ("guess", po::value<bool>(), "");
    
    po::options_description hiddenOptions;
    hiddenOptions.add_options()
        ("configuration-file", po::value<std::string>(), "");

    po::options_description visibleOptions("usage: "); // TODO: Better Description Needed
    visibleOptions.add(genericOptions).add(configOptions);

    po::options_description fullOptions;
    fullOptions.add(visibleOptions).add(hiddenOptions);

    // Create Positional Options
    po::positional_options_description positionalOptions;
    positionalOptions.add("configuration-file", 1);

    // Create Variable Map
    po::variables_map variablesMap;
    po::store(po::command_line_parser(argc, argv).options(fullOptions).positional(positionalOptions).run(), variablesMap);
    po::notify(variablesMap);

    // Print Help Message if Needed
    if (variablesMap.count("help")) {
        std::cout << visibleOptions << std::endl;
        return 0;
    }

    // Create the Q-Chem Input File
    //std::ofstream qchemFile(fmt::format("{0}/{1}/{2}", RESULTS_PATH, ))

    return 0;
}