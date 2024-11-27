#pragma once

#include <optional>

#include <toml++/toml.hpp>
#include <boost/program_options.hpp>
namespace po = boost::program_options;

struct InputFile
{
public:
    std::string FileName;

    // HPC Setup Parameters
    std::string RunFolder;

    int NumCPUs = MAX_CORES;
    bool UseGPU = false;

    double MaxHours = 48.0;

    // Molecule Parameters
    std::string MoleculeName;

    bool CreateGeometry = false;
    int GeometryStartIndex = 0;

    // Computation Parameters
    std::string Method;

    int Multiplicity;
    double Temperature;
    double TimestepDuration;

    int NumBranches = 0;
    int NumStates = 1;
    int NumTrajectories;
    int NumTimesteps;
};

class InputParser
{
public:
    InputParser(int a_countArguments, char *a_arguments[]);
    ~InputParser();

private:
    po::options_description _optionsDescription;
    po::positional_options_description _positionalOptionsDescription;

    po::variables_map _variableMap;

    std::vector<InputFile> _inputFiles;

public:
    bool PrintHelpMessage();
    bool ParseInputFiles();

    std::optional<InputFile> GetInputFile(u_int32_t a_Index);
    std::optional<InputFile> GetInputFile(std::string a_FileName);
};