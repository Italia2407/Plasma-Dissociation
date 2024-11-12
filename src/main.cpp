#include <iostream>
#include <fmt/format.h>

#include <toml++/toml.hpp>

#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include <unistd.h>
#include <limits.h>

#include <regex>

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
        std::cerr << "ERROR Occurred" << std::endl;
        return 0;
    }

    // Check Basic Arguments
    bool argCheck = true;

    if (inputFile["Setup"]["Repetitions"].type() != toml::v3::node_type::integer) {
        std::cerr << "ERROR: Setup.Repetitions must be an Integer" << std::endl;
        argCheck = false;
    }
    else if (inputFile["Setup"]["Repetitions"].value_or(0) < 1) {
        std::cerr << "ERROR: Number of Repetitions must be at least 1" << std::endl;
        argCheck = false;
    }

    if (inputFile["Setup"]["Cores"].type() != toml::v3::node_type::integer) {
        std::cerr << "ERROR: Setup.Cores must be an Integer" << std::endl;
        return 0;
    }
    else if (inputFile["Setup"]["Cores"].value_or(0) < 1) {
        std::cerr << "ERROR: Number of Cores Selected must be at least 1" << std::endl;
        argCheck = false;
    }
    else if (inputFile["Setup"]["Cores"].value_or(0) > MAX_CORES) {
        std::cerr << fmt::format("ERROR: Maximum Number of Cores available is {0}", MAX_CORES) << std::endl;
        argCheck = false;
    }

    if (inputFile["Run"]["GeomFLG"].type() != toml::v3::node_type::boolean) {
        std::cerr << "ERROR: Run.GeomFLG must be a Boolean" << std::endl;
        argCheck = false;
    }

    // 
    if (!argCheck)
        return 0;
    std::cout << "Tested Molecule: " << inputFile["Run"]["Molecule"].value_or("") << std::endl;

    // Check if Running on HPC Server
    char hostName[HOST_NAME_MAX]; gethostname(hostName, HOST_NAME_MAX);
    bool HPCFlag = std::regex_match(hostName, std::regex("login(1|2).arc(3|4).leeds.ac.uk"));

    std::cout << hostName << std::endl;

    return 1;
}