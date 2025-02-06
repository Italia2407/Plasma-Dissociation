#include <iostream>
#include <fstream>

#include <exception>
#include <algorithm>
#include <regex>

#include <unistd.h>

#include <fmt/format.h>
#include <fmt/chrono.h>
#include <argparse/argparse.hpp>

#include <filesystem>
namespace fs = std::filesystem;

#include "MoleculeProject.hpp"

static fs::directory_entry projectsDirectory;

/// @brief 
/// @param a_moleculeProject 
std::string getProjectFolder(MoleculeProject a_moleculeProject)
{
    return fmt::format("{0}/{1}", projectsDirectory.path().lexically_normal().c_str(), a_moleculeProject.ProjectName);
}

/// @brief 
/// @param a_moleculeProject 
/// @param a_restartProject 
/// @return Whether the Project Directories were Successfully Created
bool createProjectDirectories(MoleculeProject a_moleculeProject, bool a_restartProject)
{
    // Clear Project Folder if Restarts Allowed
    if (a_restartProject)
        fs::remove_all(getProjectFolder(a_moleculeProject));
    // Ensure Project isn't Ongoing
    else if (fs::exists(getProjectFolder(a_moleculeProject))) {
        std::cerr << fmt::format("ERROR: Project {0} is Currently Ongoing. Change the Project Name or Enable Project Restart", a_moleculeProject.ProjectName) << std::endl;
        return false;
    }

    // Create Project Folders
    fs::create_directories(getProjectFolder(a_moleculeProject));

    fs::create_directory(fmt::format("{0}/trajectories", getProjectFolder(a_moleculeProject)));

    for (int i = 0; i < a_moleculeProject.NumTrajectories; i++)
        fs::create_directory(fmt::format("{0}/trajectories/run-{1}", getProjectFolder(a_moleculeProject), i));

    // Create Geometries if Needed
    fs::create_directory(fmt::format("{0}/geometries", getProjectFolder(a_moleculeProject)));

    if (a_moleculeProject.CreateGeometry) {
    for (int i = 0; i < a_moleculeProject.NumTrajectories; i++) {
        std::ofstream geometryFile(fmt::format("{0}/geometries/Geom-{1}.txt", getProjectFolder(a_moleculeProject), i));
        geometryFile.close();
    }}
    else
    {
        std::cout << fmt::format("Please insert") << std::endl; // TODO: Write Message to prompt user to add Geometry Files
        std::cin.ignore();

        // Check if all Geometry files have been added
        for (int i = 0; i < a_moleculeProject.NumTrajectories; i++)
            if (!fs::exists(fmt::format("{0}/geometries/Geom-{1}.txt", getProjectFolder(a_moleculeProject), i)))
                std::cerr << fmt::format("ERROR: {0}/geometries/Geom-{1}.txt could not be found", getProjectFolder(a_moleculeProject), i) << std::endl;
    }

    return true;
}
/// @brief 
/// @param a_moleculeProject 
/// @return Whetehr the Project was Submitted to ARC
bool runProjectARC(MoleculeProject a_moleculeProject)
{
    // Check if Running on HPC Server
    char hostName[HOST_NAME_MAX];  gethostname(hostName, HOST_NAME_MAX);
    char userName[LOGIN_NAME_MAX]; getlogin_r(userName, LOGIN_NAME_MAX);

    if (!std::regex_match(hostName, std::regex("login(1|2).arc(3|4).leeds.ac.uk")))
        return false;

    // Ensure Computation Time is between Minimum (15min) and Maximum (48h)
    double maxHours = std::clamp(a_moleculeProject.MaxHours, 0.25, 48.0);

    // Write ARC Submission Script
    std::ofstream scriptFile(fmt::format("{0}/ARC_Commands.sh", getProjectFolder(a_moleculeProject)));

    // ARC Setup Instructions
    scriptFile << "#$ -cwd -V" << '\n';
    scriptFile << fmt::format("#$ -l h_vmem=4G, h_rt={:d}:{:%M:%S}", int(maxHours), std::chrono::seconds(int(maxHours * 3600))) << '\n';
    scriptFile << fmt::format("#$ -N Plasma-{0}", a_moleculeProject.ProjectName) << '\n';
    scriptFile << fmt::format("#$ -pe smp {0}", a_moleculeProject.NumCPUs) << '\n';
    scriptFile << fmt::format("#$ -t 1-{0}", a_moleculeProject.NumTrajectories) << '\n';

    // Load QChem Modules
    scriptFile << "module load test qchem" << '\n';

    scriptFile << "mkdir $TMPDIR/qchemlocal" << '\n';
    scriptFile << fmt::format("tar -xzvf /nobackup/{0}/qchem.tar.gz -C $TMPDIR/qchemlocal", userName) << '\n';
    scriptFile << "qchemlocal=$TMPDIR/qchemlocal" << '\n';

    scriptFile << "export QCHEM_HOME=\"$qchemlocal\"" << '\n';
    scriptFile << "export QC=\"$qchemlocal\"" << '\n';
    scriptFile << "export QCAUX=\"$QC/qcaux\"" << '\n';
    scriptFile << "export QCPROG=\"$QC/exe/qcprog.exe\"" << '\n';
    scriptFile << "export QCPROG_S=\"$QC/exe/qcprog.exe_s\"" << '\n';
    scriptFile << "export PATH=\"$PATH:$QC/exe:$QC/bin\"" << '\n';

    // Call Program to run Computations
    scriptFile << fmt::format("./plasma-dissociation $SGE_TASK_ID list \"{0}\"", getProjectFolder(a_moleculeProject)) << " ";
    scriptFile << fmt::format("--num_cpus {0}", a_moleculeProject.NumCPUs) << " ";
    scriptFile << fmt::format("{0} {1}", a_moleculeProject.NumStates, a_moleculeProject.NumBranches) << " ";
    scriptFile << fmt::format("{0} {1}", a_moleculeProject.NumTimesteps, a_moleculeProject.TimestepDuration) << " ";
    scriptFile << fmt::format("--theory \"{0}\"", a_moleculeProject.Theory) << " ";
    if (a_moleculeProject.SpinFlip)
        scriptFile << "--spin_flip" << " ";

    scriptFile.close();

    // Submit Job to ARC
    system("qsub -hold_jid qchemlocal ARC_Commands.sh");

    return true;
}

/// @brief 
/// @param argc Number of Command Line Arguments
/// @param argv List of Command Line Arguments
/// @return 
int main(int argc, char *argv[])
{
    // Create Argument Parser
    argparse::ArgumentParser argumentParser("PD-Setup-Run", "1.0.0"); {
        argumentParser.add_argument("input_files")
            .help("The Tested Input Files")
            .nargs(argparse::nargs_pattern::at_least_one);
        argumentParser.add_argument("-R", "--restart")
            .help("") // TODO: Add Description of Project Restart
            .flag();
        argumentParser.add_argument("-dir", "--projects_directory")
            .help("") // TODO: Add Description of Projects Directory Location
            .default_value("./projects");
    }

    // Parse Arguments from Command Line
    try {
        argumentParser.parse_args(argc, argv);
    }
    catch (const std::exception& err) {
        std::cerr << err.what() << std::endl;

        return 0;
    }

    // Ensure Projects Directory is Valid Location
    projectsDirectory = fs::directory_entry(argumentParser.get<std::string>("--projects_directory"));
    if (projectsDirectory.exists() && !projectsDirectory.is_directory()) {
        std::cerr << "ERROR: Provided Project Directory isn't Valid" << std::endl;
        return 0;
    }

    // Parse TOML Files
    std::vector<MoleculeProject> moleculeProjects;
    for (auto inputFile : argumentParser.get<std::vector<std::string>>("input_files")) {
        auto moleculeProject = MoleculeProject::CreateFromTOMLFile(inputFile);
        if (moleculeProject)
            moleculeProjects.push_back(moleculeProject.value());
    }
    // Ensure at least one File was Parsed Correctly
    if (moleculeProjects.size() < 1)
        return 0;
    
    // TODO: Add User Confirmation for Continuing with not all Files Correctly Parsed

    for (auto moleculeProject : moleculeProjects) {
        std::cout << "Tested Molecule: " << moleculeProject.MoleculeName << std::endl;

        // Create Directories of Project, if Possible
        if (!createProjectDirectories(moleculeProject, argumentParser.get<bool>("--restart")))
            continue;

        // Submit Project Job to ARC System
        if (!runProjectARC(moleculeProject))
            std::cout << "Code not Running on ARC System" << std::endl;
    }

    return 1;
}