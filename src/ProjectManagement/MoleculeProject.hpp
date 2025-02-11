#pragma once

#include <string>
#include <optional>

#include <filesystem>
namespace fs = std::filesystem;

struct MoleculeProject
{
    // HPC Setup Parameters
    std::string ProjectName;

    int NumCPUs;
    bool UseGPU;

    double MaxHours;

    // Molecule Parameters
    std::string MoleculeName;
    bool CreateGeometry;

    // Computation Parameters
    std::string Method;
    std::string Theory;
    bool SpinFlip;

    int Multiplicity;
    double Temperature;
    double TimestepDuration;

    int NumBranches;
    int NumStates;
    int NumTrajectories;
    int NumTimesteps;

public:
    bool AreValuesValid() const;

    // Builder Methods
    static std::optional<MoleculeProject> CreateFromTOMLFile(std::string a_filePath);
};