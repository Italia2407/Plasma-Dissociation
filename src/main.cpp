#include <iostream>
#include <exception>

#include <fmt/format.h>
#include <argparse/argparse.hpp>

#include <filesystem>
namespace fs = std::filesystem;

#include "MoleculeTest.hpp"

/// @brief 
/// @param argc Number of Command Line Arguments
/// @param argv List of Command Line Arguments
/// @return 
int main(int argc, char *argv[])
{
    // Create Argument Parser
    argparse::ArgumentParser argumentParser("Plasma-Dissociation", "1.0.0"); {
        argumentParser.add_argument("input-files")
            .help("The Tested Input Files")
            .nargs(argparse::nargs_pattern::at_least_one);
    }

    // Parse Arguments from Command Line
    try {
        argumentParser.parse_args(argc, argv);
    }
    catch (const std::exception& err) {
        std::cerr << err.what() << std::endl;

        return 0;
    }

    // Parse TOML Files
    std::vector<MoleculeTest> moleculeTests;
    for (auto inputFile : argumentParser.get<std::vector<std::string>>("input-files")) {
        auto moleculeTest = MoleculeTest::CreateFromTOMLFile(inputFile);
        if (moleculeTest)
            moleculeTests.push_back(moleculeTest.value());
    }
    // Ensure at least one File was Parsed Correctly
    if (moleculeTests.size() < 1)
        return 0;

    // TODO: Add User Confirmation for Continuing with not all Files Correctly Parsed
    std::cout << "Tested Molecule: " << moleculeTests[0].MoleculeName << std::endl;

    // Get Results Folder Path
    fs::directory_entry runFolder(moleculeTests[0].RunFolder);

    // Ensure Directory does not Exist
    if (runFolder.is_directory()) {
        std::cerr << "ERROR: RunFolder already Exists. Change the RunFolder Name or delete/move it" << std::endl;
        return 0;
    }

    // Create the RunFolder
    fs::create_directories(runFolder);

    return 1;
}