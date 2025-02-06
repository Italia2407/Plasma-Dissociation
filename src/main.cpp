#include <iostream>

#include <argparse/argparse.hpp>
#include <fmt/format.h>

#include <filesystem>
namespace fs = std::filesystem;

#include "MoleculeProject.hpp"

static std::string projectFolder;
static MoleculeProject moleculeProject;

int main(int argc, char *argv[])
{
    // Create Argument Parsers #TODO: Write Argument Descriptions
    // Create Argument Parsers
    argparse::ArgumentParser listParser("list"); {
        listParser.add_argument("project_folder")
            .help("Folder containing the Project to be Run");

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
        fileParser.add_argument("-dir", "--projects_directory")
            .help("")
            .default_value("./projects");
    }
    fileParser.add_description("");

    argparse::ArgumentParser argumentParser("Plasma-Dissociation", "1.0.0"); {
        argumentParser.add_subparser(listParser);
        argumentParser.add_subparser(fileParser);

        argumentParser.add_argument("--id_trajectory")
            .help("ID of the Trajectory to be Run")
            .scan<'i', int>()
            .default_value(0);
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
        projectFolder = listParser.get<std::string>("project_folder");

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
        // Ensure Projects Directory is Valid Location
        auto projectsDirectory = fs::directory_entry(fileParser.get<std::string>("--projects_directory"));
        if (projectsDirectory.exists() && !projectsDirectory.is_directory()) {
            std::cerr << "ERROR: Provided Project Directory isn't Valid" << std::endl;
            return 0;
        }

        // Parse TOML Project File
        auto projectFile = MoleculeProject::CreateFromTOMLFile(fileParser.get<std::string>("project_file"));
        if (!projectFile)
            return 0;
        moleculeProject = projectFile.value();

        projectFolder = fmt::format("{0}/{1}", projectsDirectory.path().lexically_normal().c_str(), moleculeProject.ProjectName);

        std::cout << "Hello World!" << std::endl;
    }
    else {
        std::cout << argumentParser << std::endl;
        return 0;
    }

    // Enter Specific Trajectory Folder
    std::string path = fmt::format("{0}/trajectories/run-{1}", projectFolder, argumentParser.get<int>("--id_trajectory"));
    fs::current_path(path);

    return 1;
}