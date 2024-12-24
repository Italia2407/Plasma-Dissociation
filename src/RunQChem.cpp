#include <iostream>
#include <fstream>

#include <fmt/format.h>

#include <argparse/argparse.hpp>

#include "IOManagers/QChemConfig.hpp"

#define QCHEM_INPUT_FILE  "qchem.inp"
#define QCHEM_OUTPUT_FILE "qchem.out"

/// @brief 
/// @param a_qchemConfig 
/// @return Whether the QChem Input File was Successfully Created
bool createQChemInput(QChemConfig a_qchemConfig)
{
    // Open the Geometry File
    std::ifstream geometryFile(a_qchemConfig.GeometryInput);
    if (!geometryFile.is_open()) {
        std::cerr << fmt::format("ERROR: Could not Open Geometry File {0}", a_qchemConfig.GeometryInput) << std::endl;

        return false;
    }

    // Create QChem Input File
    std::ofstream qchemInputFile(QCHEM_INPUT_FILE);
    if (!qchemInputFile.is_open()) {
        std::cerr << fmt::format("ERROR: Could not Create QChem Input File {0}", QCHEM_INPUT_FILE) << std::endl;

        return false;
    }

    // Insert Data in QChem Input File
    qchemInputFile << "$molecule" << '\n';
    qchemInputFile << geometryFile.rdbuf() << '\n';
    qchemInputFile << "$end" << '\n';

    qchemInputFile << "$rem" << '\n';
    // TODO: Add more Options for Customisation, e.g. JOBTYPE, BASIS, etc...
    qchemInputFile << '\t' << fmt::format("JOBTYPE {0}", "FORCE") << '\n';
    qchemInputFile << '\t' << fmt::format("EXCHANGE {0}", "BHHLYP") << '\n';
    qchemInputFile << '\t' << fmt::format("BASIS {0}", "6-31+G*") << '\n';
    qchemInputFile << '\t' << fmt::format("UNRESTRICTED {0}", "True") << '\n';
    qchemInputFile << '\t' << fmt::format("MAX_SCF_CYCLES {0}", 500) << '\n';
    qchemInputFile << '\t' << fmt::format("SYM_IGNORE {0}", "True") << '\n';

    qchemInputFile << '\t' << fmt::format("SCF_Algorithm {0}", a_qchemConfig.SCFAlgorithm) << '\n';
    qchemInputFile << '\t' << fmt::format("SPIN_FLIP {0}", a_qchemConfig.SpinFlip ? "True" : "False") << '\n';
    qchemInputFile << '\t' << fmt::format("SET_Iter {0}", 500) << '\n';
    qchemInputFile << '\t' << fmt::format("MAX_CIS_CYCLES {0}", 500) << '\n';

    if (a_qchemConfig.Guess)
        qchemInputFile << '\t' << fmt::format("SCF_GUESS {0}", "Read") << '\n';

    if (a_qchemConfig.Theory == 1) {
        qchemInputFile << '\t' << fmt::format("CIS_N_ROOTS {0}", 1) << '\n';
        qchemInputFile << '\t' << fmt::format("CIS_STATE_DERIV {0}", 1) << '\n';
    }

    qchemInputFile << "$end" << '\n';

    // Close the Opened Files
    geometryFile.close();
    qchemInputFile.close();

    return true;
}

/// @brief 
/// @param a_numCPUs The Number of CPUs with which to Run the Job
/// @return Whether the Job was Successfully Submitted
bool submitQChemJob(int a_numCPUs)
{
    // Create the CLI Command String
    std::string qchemCommand = fmt::format("qchem -save -nt {0} \"{1}\" \"{2}\" wf", a_numCPUs, QCHEM_INPUT_FILE, QCHEM_OUTPUT_FILE);

    // Submit the CLI Command
    system(qchemCommand.c_str());

    return true;
}

int main(int argc, char *argv[])
{
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

    // Parse TOML File
    std::string qchemConfigFile = argumentParser.get<std::string>("configuration-file");
    auto qchemConfig = QChemConfig::CreateFromTOMLFile(qchemConfigFile);

    // Ensure Configuration File was Parsed Correctly
    if (!qchemConfig.has_value())
        return 0;

    // Submit QChem Job to Run
    createQChemInput(qchemConfig.value());
    submitQChemJob(qchemConfig.value().NumCPUs);
    //std::ofstream qchemFile(fmt::format("{0}/{1}/{2}", RESULTS_PATH, ))

    return 1;
}