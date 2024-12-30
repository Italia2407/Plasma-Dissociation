#include "QChemManagement.hpp"

#define QCHEM_INPUT_FILE  "qchem.inp"
#define QCHEM_OUTPUT_FILE "qchem.out"

#include <iostream>
#include <fstream>

#include <string>
#include <fmt/format.h>

namespace QChemManagement {

/// @brief 
/// @param a_qchemConfig 
/// @return Whether the QChem Input File was Successfully Created
bool CreateQChemInput(QChemConfig a_qchemConfig)
{
    // Open the Molecule Geometry File
    std::ifstream geometryFile(a_qchemConfig.GeometryInput);
    if (!geometryFile.is_open()) {
        std::cerr << fmt::format("ERROR: Could not Open Geometry File {0}", a_qchemConfig.GeometryInput) << std::endl;

        return false;
    }

    // Create QChem Input File
    std::string qchemInputFilePath = fmt::format("{0}/{1}", a_qchemConfig.RunFolder, QCHEM_INPUT_FILE);
    std::ofstream qchemInputFile(qchemInputFilePath);
    if (!qchemInputFile.is_open()) {
        std::cerr << fmt::format("ERROR: Could not Create QChem Input File {0}", qchemInputFilePath) << std::endl;

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
/// @param a_qchemInputFile The Path to the QChem Input File
/// @param a_qchemOutputFile The Path to the QChem Output File
/// @param a_numCPUs The Number of CPUs with which to Run QChem
void RunQChem(std::string a_qchemInputFile, std::string a_qchemOutputFile, int a_numCPUs)
{
    // Create the CLI Command String
    std::string qchemCommand = fmt::format("qchem -save -nt {0} \"{1}\" \"{2}\" wf", a_numCPUs, a_qchemInputFile, a_qchemOutputFile);

    // Submit the CLI Command
    system(qchemCommand.c_str());
}

/// @brief 
/// @param a_qchemConfig 
/// @return Whether the QChem Job was Successfully Submitted
bool SubmitQChemJob(QChemConfig a_qchemConfig)
{
    // Create the QChem Input
    if (!CreateQChemInput(a_qchemConfig))
        return false;

    // Run QChem CLI Command
    std::string qchemInputFilePath = fmt::format("{0}/{1}", a_qchemConfig.RunFolder, QCHEM_INPUT_FILE);
    std::string qchemOutputFilePath = fmt::format("{0}/{1}", a_qchemConfig.RunFolder, QCHEM_OUTPUT_FILE);

    RunQChem(qchemInputFilePath, qchemOutputFilePath, a_qchemConfig.NumCPUs);
    return true;
}

}