#include <iostream>
#include <fmt/format.h>

#include <unistd.h>
#include <limits.h>

#include <filesystem>
namespace fs = std::filesystem;

#include <regex>

#include "../InputParser.hpp"

/// @brief 
/// @param a_inputFile 
/// @return 
bool argumentValidator(toml::v3::ex::parse_result a_inputFile)
{
    bool argCheck = true;

    // Check if Value of Trajectories is Valid
    if (a_inputFile["InitConditions"]["NumTrajectories"].type() != toml::v3::node_type::integer) {
        std::cerr << "ERROR: InitConditions.NumTrajectories must be an Integer" << std::endl;
        argCheck = false;
    }
    else if (a_inputFile["InitConditions"]["NumTrajectories"].value_or(0) < 1) {
        std::cerr << "ERROR: Number of Trajectories must be at least 1" << std::endl;
        argCheck = false;
    }

    // Check if Value of CPUs is Valid
    if (a_inputFile["HPCSetup"]["NumCPUs"].type() != toml::v3::node_type::integer) {
        std::cerr << "ERROR: HPCSetup.NumCPUs must be an Integer" << std::endl;
        argCheck = false;
    }
    else if (a_inputFile["HPCSetup"]["NumCPUs"].value_or(0) < 1) {
        std::cerr << "ERROR: Number of CPUs Selected must be at least 1" << std::endl;
        argCheck = false;
    }
    else if (a_inputFile["HPCSetup"]["NumCPUs"].value_or(0) > MAX_CORES) {
        std::cerr << fmt::format("ERROR: Maximum Number of CPUs available is {0}", MAX_CORES) << std::endl;
        argCheck = false;
    }

    // Check if CreateGeometry Flag is Valid
    if (a_inputFile["Molecule"]["CreateGeometry"].type() != toml::v3::node_type::boolean) {
        std::cerr << "ERROR: Molecule.CreateGeometry must be a Boolean" << std::endl;
        argCheck = false;
    }

    return argCheck;
}

int main(int argc, char *argv[])
{
    // Initialise File Parser
    InputParser inputParser(argc, argv);
    if (inputParser.PrintHelpMessage())
        return 0;

    // Ensure File Parsing is Successful
    if (!inputParser.ParseInputFiles())
        return 0;

    // Retrieve First Parsed File
    if (!inputParser.GetInputFile(0).has_value()) {
        std::cerr << "ERROR: Could not Retrieve Input File 0" << std::endl;
        return 0;
    }
    InputFile inputFile = inputParser.GetInputFile(0).value();
    std::cout << "Tested Molecule: " << inputFile.MoleculeName << std::endl;

    // Check if Running on HPC Server
    char hostName[HOST_NAME_MAX];  gethostname(hostName, HOST_NAME_MAX);
    char userName[LOGIN_NAME_MAX]; getlogin_r(userName, LOGIN_NAME_MAX);

    bool HPCFlag = std::regex_match(hostName, std::regex("login(1|2).arc(3|4).leeds.ac.uk"));

    // Create Execution Directory
    fs::directory_entry execDir(HPCFlag ? fmt::format("/nobackup/{0}/Plasma-Dissociation-Results", userName) : "../../results");
    if (!execDir.is_directory())
        fs::create_directory(execDir);

    fs::directory_entry runFolder(fmt::format("{0}/{1}", execDir.path().string(), inputFile.RunFolder));
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
    fs::create_directory(runFolder);

    return 1;
}