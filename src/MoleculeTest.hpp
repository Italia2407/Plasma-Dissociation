#pragma once

#include <string>
#include <optional>

struct MoleculeTest
{
    // HPC Setup Parameters
    std::string ProjectName;

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
    static std::optional<MoleculeTest> CreateFromTOMLFile(std::string a_filePath);
};