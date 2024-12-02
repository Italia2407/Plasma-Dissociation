#pragma once

#include <string>
#include <optional>

#include <toml++/toml.hpp>

struct MoleculeTest
{
public:
    // HPC Setup Parameters
    std::string RunFolderName;
    std::string GetRunFolderPath() const;

    int NumCPUs;
    bool UseGPU;

    double MaxHours;

    // Molecule Parameters
    std::string MoleculeName;

    bool CreateGeometry;
    int GeometryStartIndex;

    // Computation Parameters
    std::string Method;

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
    static std::optional<MoleculeTest> CreateFromTOMLFile(std::string a_fileName);
};