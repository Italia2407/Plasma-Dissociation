#include <iostream>
#include <exception>

#include <fmt/format.h>

#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include <filesystem>
namespace fs = std::filesystem;

#include "IOManagers/MoleculeTest.hpp"

int main(int argc, char *argv[])
{
    // Create Command Line Options
    po::options_description visibleOptionsDescription("Invocation: <program> --help <input-files>"); // TODO: Better Description Needed
    visibleOptionsDescription.add_options()
        ("help", "Produce a Help Message")
        ("version,v", "Display the Program's Version");
    
    po::options_description hiddenOptionsDescription;
    hiddenOptionsDescription.add_options()
        ("input-files", po::value<std::vector<std::string>>(), "Tested Input Files");

    po::options_description optionsDescription;
    optionsDescription.add(visibleOptionsDescription).add(hiddenOptionsDescription);

    // Create Positional Options
    po::positional_options_description positionalOptionsDescription;
    positionalOptionsDescription.add("input-files", -1);

    // Create Variable Map
    po::variables_map variablesMap;
    po::store(po::command_line_parser(argc, argv).options(optionsDescription).positional(positionalOptionsDescription).run(), variablesMap);
    po::notify(variablesMap);

    // Print Help Message if Needed
    if (variablesMap.count("help")) {
        std::cout << visibleOptionsDescription << std::endl;
        return 0;
    }

    // Initialise Molecule Tests
    if (variablesMap.count("input-files") < 1) {
        std::cerr << "ERROR: At least one Input File must be Provided" << std::endl;
        return 0;
    }

    std::vector<MoleculeTest> moleculeTests;
    for (auto inputFile : variablesMap["input-files"].as<std::vector<std::string>>()) {
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
    fs::directory_entry runFolder(fmt::format("{0}/{1}", RESULTS_PATH, moleculeTests[0].GetRunFolderPath()));

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