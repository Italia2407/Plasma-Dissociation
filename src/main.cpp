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

#include "MoleculeTest.hpp"

bool createProjectDirectories(MoleculeTest a_moleculeTest, bool a_restartProject = false)
{
    // Get the Project's RunFolder
    std::string runFolder = fmt::format("projects/{0}", a_moleculeTest.ProjectName);

    // Clear Project Folder if Restarts Allowed
    if (a_restartProject)
        fs::remove_all(runFolder);
    // Ensure Project isn't Ongoing
    else if (fs::exists(runFolder)) {
        std::cerr << fmt::format("ERROR: Project {0} is Currently Ongoing. Change the Project Name or Enable Project Restart", a_moleculeTest.ProjectName) << std::endl;
        return false;
    }

    // Create Project Folders
    fs::create_directories(runFolder);

    fs::create_directory(fmt::format("{0}/trajectories", runFolder));

    for (int i = 0; i < a_moleculeTest.NumTrajectories; i++)
        fs::create_directory(fmt::format("{0}/trajectories/run-{1}", runFolder, i+1));

    // Create Geometries if Needed
    fs::create_directory(fmt::format("{0}/geometries", runFolder));

    if (a_moleculeTest.CreateGeometry) {
    for (int i = 0; i < a_moleculeTest.NumTrajectories; i++) {
        std::ofstream geometryFile(fmt::format("{0}/geometries/Geom-{1}.txt", runFolder, i+1));
        geometryFile.close();
    }}
    else
    {
        std::cout << fmt::format("Please insert") << std::endl; // TODO: Write Message to prompt user to add Geometry Files
        std::cin.ignore();

        // Check if all Geometry files have been added
        for (int i = 0; i < a_moleculeTest.NumTrajectories; i++)
            if (!fs::exists(fmt::format("{0}/geometries/Geom-{1}.txt", runFolder, i+1)))
                std::cerr << fmt::format("ERROR: {0}/geometries/Geom-{1}.txt could not be found", runFolder, i+1) << std::endl;
    }

    return true;
}
bool runProjectARC(MoleculeTest a_moleculeTest)
{
    // Check if Running on HPC Server
    char hostName[HOST_NAME_MAX];  gethostname(hostName, HOST_NAME_MAX);
    char userName[LOGIN_NAME_MAX]; getlogin_r(userName, LOGIN_NAME_MAX);

    if (!std::regex_match(hostName, std::regex("login(1|2).arc(3|4).leeds.ac.uk")))
        return false;

    // Ensure Computation Time is between Minimum (15min) and Maximum (48h)
    double maxHours = std::clamp(a_moleculeTest.MaxHours, 0.25, 48.0);

    // Write ARC Submission Script
    std::ofstream scriptFile(fmt::format("projects/{0}/ARC_Commands.sh", a_moleculeTest.ProjectName));

    // ARC Setup Instructions
    scriptFile << "#$ -cwd -V" << '\n';
    scriptFile << fmt::format("#$ -l h_vmem=4G, h_rt={:d}:{:%M:%S}", int(maxHours), std::chrono::seconds(int(maxHours * 3600))) << '\n';
    scriptFile << fmt::format("#$ -N Plasma-{0}", a_moleculeTest.ProjectName) << '\n';
    scriptFile << fmt::format("#$ -pe smp {0}", a_moleculeTest.NumCPUs) << '\n';
    scriptFile << fmt::format("#$ -t 1-{0}", a_moleculeTest.NumTrajectories) << '\n';

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

    scriptFile << fmt::format("cd projects/{0}/trajectories/run-$SGE_TASK_ID", a_moleculeTest.ProjectName) << '\n';
    // TODO: Call Program to run Computations

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
    // TODO: Add Restart Argument
    // Restart Clears the Folder so that the Project is ran from Scratch

    // Create Argument Parser
    argparse::ArgumentParser argumentParser("Plasma-Dissociation", "1.0.0"); {
        argumentParser.add_argument("input-files")
            .help("The Tested Input Files")
            .nargs(argparse::nargs_pattern::at_least_one);
        argumentParser.add_argument("-R", "--restart")
            .help("") // TODO: Add Description of Project Restart
            .flag();
    }

    // Parse Arguments from Command Line
    try {
        argumentParser.parse_args(argc, argv);
    }
    catch (const std::exception& err) {
        std::cerr << err.what() << std::endl;

        return 0;
    }

    // Parse TOML Files
    std::vector<MoleculeTest> moleculeTests;
    for (auto inputFile : argumentParser.get<std::vector<std::string>>("input-files")) {
        auto moleculeTest = MoleculeTest::CreateFromTOMLFile(inputFile);
        if (moleculeTest)
            moleculeTests.push_back(moleculeTest.value());
    }
    // Ensure at least one File was Parsed Correctly
    if (moleculeTests.size() < 1)
        return 0;
    
    // TODO: Add User Confirmation for Continuing with not all Files Correctly Parsed

    for (auto moleculeTest : moleculeTests) {
        std::cout << "Tested Molecule: " << moleculeTest.MoleculeName << std::endl;

        // Create Directories of Project, if Possible
        if (!createProjectDirectories(moleculeTest, argumentParser.get<bool>("--restart")))
            continue;

        // Submit Project Job to ARC System
        if (!runProjectARC(moleculeTest))
            std::cout << "Code not Running on ARC System" << std::endl;
    }

    return 1;
}