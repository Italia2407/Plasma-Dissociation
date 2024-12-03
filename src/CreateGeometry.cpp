#include <iostream>

#include <boost/program_options.hpp>
namespace po = boost::program_options;

int main(int argc, char *argv[])
{
    // Create Command Line Options
    po::options_description genericOptions("Generic Options");
    genericOptions.add_options()
        ("help", "Produce a Help Message")
        ("version,v", "Display the Program's Version");

    po::options_description hiddenOptions;
    hiddenOptions.add_options()
        ("molecule", po::value<std::string>(), "Molecule to Create the Geometry of");

    po::options_description visibleOptions("usage: <program> --help <molecule>"); // TODO: Better Description Needed
    visibleOptions.add(genericOptions);

    po::options_description fullOptions;
    fullOptions.add(visibleOptions).add(hiddenOptions);

    // Create Positional Options
    po::positional_options_description positionalOptions;
    positionalOptions.add("molecules", 1);

    // Create Variable Map
    po::variables_map variablesMap;
    po::store(po::command_line_parser(argc, argv).options(fullOptions).positional(positionalOptions).run(), variablesMap);
    po::notify(variablesMap);

    // Print Help Message if Needed
    if (variablesMap.count("help")) {
        std::cout << visibleOptions << std::endl;
        return 0;
    }

    std::cout << "I'm Creating the Geometries" << std::endl;
    
    return 0;
}