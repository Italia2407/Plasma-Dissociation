#include <iostream>
#include <fstream>

#include <string>
#include <fmt/format.h>

#include <argparse/argparse.hpp>
#include <fmt/format.h>

#include <filesystem>
namespace fs = std::filesystem;

#include "ProjectManagement/MoleculeProject.hpp"

static MoleculeProject moleculeProject;

bool processGeometryFile(int a_trajectoryID, MoleculeProject a_moleculeProject)
{
    // Open Molecule Geometry File
    std::ifstream geometryFile(fmt::format("../"));
    if (!geometryFile.is_open()) {
        //std::cerr << fmt::format("ERROR: Could not Open Geometry File {0}", a_qchemConfig.GeometryInput) << std::endl;

        return false;
    }

    return true;
}

int main(int argc, char *argv[])
{
    // Create Argument Parsers #TODO: Write Argument Descriptions
    argparse::ArgumentParser listParser("list"); {
        listParser.add_argument("num_states")
            .help("")
            .scan<'i', int>();
        listParser.add_argument("num_branches")
            .help("")
            .scan<'i', int>();
        listParser.add_argument("num_timesteps")
            .help("")
            .scan<'i', int>();

        listParser.add_argument("timestep_duration")
            .help("Duration for ")
            .scan<'g', double>();
        
        listParser.add_argument("--num_cpus")
            .help("Max Number of CPUs to use duirng Calculations")
            .scan<'i', int>()
            .default_value(MAX_CORES);
        listParser.add_argument("--theory")
            .help("")
            .default_value("SCF");
        listParser.add_argument("-sf", "--spin_flip")
            .help("")
            .flag();
    }
    listParser.add_description("");

    argparse::ArgumentParser fileParser("file"); {
        fileParser.add_argument("project_file")
            .help("Input File")
            .nargs(argparse::nargs_pattern::at_least_one);
    }
    fileParser.add_description("");

    argparse::ArgumentParser argumentParser("Plasma-Dissociation", "1.0.0"); {
        argumentParser.add_subparser(listParser);
        argumentParser.add_subparser(fileParser);

        argumentParser.add_argument("--id_trajectory")
            .help("ID of the Trajectory to be Run")
            .scan<'i', int>()
            .default_value(0);
        argumentParser.add_argument("-dir", "--run_directory")
            .help("Directory in Which the Program should be Run")
            .default_value(".");
    }

    // Parse Arguments from Command Line
    try {
        argumentParser.parse_args(argc, argv);
    }
    catch (const std::exception& err) {
        std::cerr << err.what() << std::endl;

        return 0;
    }

    // Store Input Parameters
    if (argumentParser.is_subcommand_used(listParser)) {
        moleculeProject.NumStates = listParser.get<int>("num_states");
        moleculeProject.NumBranches = listParser.get<int>("num_branches");
        moleculeProject.NumTimesteps = listParser.get<int>("num_timesteps");

        moleculeProject.TimestepDuration = listParser.get<double>("timestep_duration");

        moleculeProject.NumCPUs = listParser.get<int>("--num_cpus");

        moleculeProject.Theory = listParser.get<std::string>("--theory");
        moleculeProject.SpinFlip = listParser.get<bool>("--spin_flip");

        std::cout << "World, Hello!" << std::endl;
    }
    else if (argumentParser.is_subcommand_used(fileParser)) {
        // Parse TOML Project File
        auto projectFile = MoleculeProject::CreateFromTOMLFile(fileParser.get<std::string>("project_file"));
        if (!projectFile)
            return 0;
        moleculeProject = projectFile.value();

        std::cout << "Hello World!" << std::endl;
    }
    else {
        std::cout << argumentParser << std::endl;
        return 0;
    }

    // Enter Run Directory
    fs::current_path(argumentParser.get<std::string>("--run_directory"));

    //std::string path = fmt::format("trajectories/run-{0}", argumentParser.get<int>("--id_trajectory"));
    //fs::current_path(path);

    return 1;
}