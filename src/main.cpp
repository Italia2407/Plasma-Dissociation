#include <iostream>
#include <fmt/format.h>

#include <toml++/toml.hpp>

#include <boost/program_options.hpp>
namespace po = boost::program_options;

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

    auto test = variableMap["input-files"].as<std::vector<std::string>>()[0];
    std::string filePath = fmt::format("{0}/Molecule-Tests/{1}", ASSETS_PATH, variableMap["input-files"].as<std::vector<std::string>>()[0]);
    auto testFile = toml::parse_file(filePath);
    std::cout << "The tested molecule is " << testFile["Run"]["Molecule"].value_or("") << std::endl;

    return 1;
}