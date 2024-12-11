#include <iostream>
#include <fstream>

#include <fmt/format.h>

#include <argparse/argparse.hpp>

#include "IOManagers/QChemConfig.hpp"

#define QCHEM_INPUT_FILE  "f.inp"
#define QCHEM_OUTPUT_FILE "f.out"

/// @brief 
/// @param a_qchemConfig 
/// @return Whether the Job was Submitted Successfully
bool SubmitQChemJob(QChemConfig a_qchemConfig)
{
    return false;
}

int main(int argc, char *argv[])
{
    system("echo testing");

    // Create Argument Parser
    argparse::ArgumentParser argumentParser("Run-QChem", "1.0.0"); {
        argumentParser.add_argument("configuration-file")
            .help("")
            .nargs(1);
    }

    // Parse Arguments from Command Line
    try {
        argumentParser.parse_args(argc, argv);
    }
    catch (const std::exception& err) {
        std::cerr << err.what() << std::endl;

        return 0;
    }

    // Create the Q-Chem Input File
    //std::ofstream qchemFile(fmt::format("{0}/{1}/{2}", RESULTS_PATH, ))

    return 1;
}