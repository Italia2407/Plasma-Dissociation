#include <iostream>

#include <argparse/argparse.hpp>

int main(int argc, char *argv[])
{
    // Create Argument Parser
    argparse::ArgumentParser argumentParser("Plasma-Dissociation", "1.0.0"); {
        argumentParser.add_argument("molecule")
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

    std::cout << "I'm Creating the Geometries" << std::endl;
    
    return 0;
}