#include <iostream>
#include <exception>

#include <fmt/format.h>

#include <argparse/argparse.hpp>

#include <filesystem>
namespace fs = std::filesystem;

#include "IOManagers/MoleculeTest.hpp"

/// @brief Program's Main Function
/// @param argc Number of Command Line Arguments Passed
/// @param argv Command Line Arguments Passed
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
        std::string inputFilePath = fmt::format("{0}/Molecule-Tests/{1}", ASSETS_PATH, inputFile);
        auto moleculeTest = MoleculeTest::CreateFromTOMLFile(inputFilePath);
        if (moleculeTest)
            moleculeTests.push_back(moleculeTest.value());
    }
    // Ensure at least one File was Parsed Correctly
    if (moleculeTests.size() < 1)
        return 0;

    // TODO: Add User Confirmation for Continuing with not all Files Correctly Parsed
    std::cout << "Tested Molecule: " << moleculeTests[0].MoleculeName << std::endl;

    // Get Results Folder Path
    fs::directory_entry runFolder(fmt::format("{0}/{1}", RESULTS_PATH, moleculeTests[0].RunFolder));

    if (runFolder.is_directory()) {
        // Check if Existing RunFolder should be Deleted
        std::cout << "Run Folder already Exists. Do you want to delete it? (Y/n): ";
        std::string cliInput; std::getline(std::cin, cliInput);

        if (cliInput == "Y" || cliInput == "y")
            fs::remove(runFolder);
        else {
            std::cerr << "ERROR: RunFolder already Exists. Change the RunFolder Name or delete/move it" << std::endl;
            return 0;
        }
    }

    // Create the RunFolder
    fs::create_directories(runFolder);

    return 1;
}