#include <iostream>
#include <fmt/format.h>

#include <toml++/toml.hpp>

#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include <unistd.h>
#include <limits.h>

#include <filesystem>
namespace fs = std::filesystem;

#include <regex>

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
    // Create Command Line Options
    po::options_description optionsDescription("Program Options");
    optionsDescription.add_options()
        ("help", "Produce a Help Message")
        ("input-files", po::value<std::vector<std::string>>(), "Tested Input Files");
    // Create Positional Options
    po::positional_options_description positionalOptionsDescription;
    positionalOptionsDescription.add("input-files", -1);

    // Create Variable Map
    po::variables_map variableMap;
    po::store(po::command_line_parser(argc, argv).options(optionsDescription).positional(positionalOptionsDescription).run(), variableMap);
    po::notify(variableMap);

    // Print CLI Help Message
    if (variableMap.count("help")) {
        std::cout << optionsDescription << std::endl;
        return 1;
    }

    // Ensure Input Files have been Passed
    if (variableMap.count("input-files") == 0) {
        std::cerr << "ERROR: At least one Input File is Required" << std::endl;
        return 0;
    }

    // Parse Inputted TOML File
    toml::v3::ex::parse_result inputFile;
    try {
        std::string filePath = fmt::format("{0}/Molecule-Tests/{1}", ASSETS_PATH, variableMap["input-files"].as<std::vector<std::string>>()[0]);
        inputFile = toml::parse_file(filePath);
    }
    catch(const toml::parse_error& error) {
        std::cerr << error.what() << std::endl;
        return 0;
    }

    // Validate Arguments from Input File
    if (!argumentValidator(inputFile))
        return 0;
    std::cout << "Tested Molecule: " << inputFile["Molecule"]["Name"].value_or("") << std::endl;

    // Check if Running on HPC Server
    char hostName[HOST_NAME_MAX];  gethostname(hostName, HOST_NAME_MAX);
    char userName[LOGIN_NAME_MAX]; getlogin_r(userName, LOGIN_NAME_MAX);

    bool HPCFlag = std::regex_match(hostName, std::regex("login(1|2).arc(3|4).leeds.ac.uk"));

    // Create Execution Directory
    fs::directory_entry execDir(HPCFlag ? fmt::format("/nobackup/{0}/Plasma-Dissociation-Results", userName) : "../../results");
    if (!execDir.is_directory())
        fs::create_directory(execDir);

    // Create RunFolder
    fs::directory_entry runFolder(fmt::format("{0}/{1}", execDir.path().string(), inputFile["HPCSetup"]["RunFolder"].value_or("default")));
    if (runFolder.is_directory())
    {
        // Check if Existing RunFolder should be Deleted
        std::cout << "Run Folder already Exists. Do you want to delete it? (Y/n): ";
        std::string cliInput; std::getline(std::cin, cliInput);

        if (cliInput == "Y" || cliInput == "y")
            fs::remove(runFolder);
        else
        {
            std::cerr << "ERROR: RunFolder already Exists. Change the RunFolder Name or delete/move it" << std::endl;
            return 0;
        }
    }

    std::cout << hostName << std::endl;

    return 1;
}